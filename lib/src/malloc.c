// this is the reason i have no friends
// i'm currently sacrificing small optimisations for readability. this hurts, especially since i can't actually compile with -O >= 2 bc for whatever reason it just breaks

#include "../mem.h"
#include "../files.h"

#define DEBUG

//for debug
#include "../io.h"

// TODO: replace
#define MALLOC_PAGE_SIZE PAGES(1)

#define PAGE_TYPE_NORM 1
#define PAGE_TYPE_LONG 2

#define BLOCK_TYPE_FREE 0
#define BLOCK_TYPE_USED 1

typedef struct malloc_page_header_s {
	u8 page_type;			// normal, long
	u16 usages;				// number of times malloc has been called on this page
	void* next_page;
	void* prev_page;
} page_header;
typedef struct malloc_block_header_s {
	u8 block_type;			// free, used
	u64 block_size;
} block_header;

typedef struct malloc_node_s {
	block_header* addr;		// address of its header
	void* next;				// next node on list
} malloc_node_t;

#define PHEADERSIZE sizeof(page_header)
#define BHEADERSIZE sizeof(block_header)
#define NODESIZE	sizeof(malloc_node_t)

// points to the first byte of the header.
// we almost never reference first_page, all we care about is wether there's
// space in the current (other pages are referenced via fragments)
void* first_page_ptr = nullptr;

// list of all memory areas that have been `free`ed
malloc_node_t* fragments_list_head = nullptr;
// s64 nodes_in_current_page;

// point to first byte of respective headers
void* current_page = nullptr;
void* current_block = nullptr;		//  \___ keep both for ease of calculation
s64 current_page_free_bytes;		//  /


void insert_fragment(block_header* block) {
	*block = (block_header) { BLOCK_TYPE_FREE, (current_page_free_bytes - sizeof(block_header)) };

	malloc_node_t* 	cur  = fragments_list_head,
					new;							// TODO: oh fuck, this is in the stack. problem for future me

	if (cur == nullptr) {
		new = (malloc_node_t){ block, nullptr };
		fragments_list_head = &new;
		return;
	}

	while(cur->next) {
		if (((malloc_node_t*)(cur->next))->addr->block_size > block) {
			break;
		}
		cur = cur->next;
	}

	new.addr = block;
	new.next = cur->next;
	cur->next = &new;

}

#define debug_msg(x) print("\tIN MALLOC: " x "\n");
#define debug_msg_int(i) print("\tIN MALLOC: "); printint(i); newl();
#define debug_msg_addr(addr) print("\tIN MALLOC: "); printhex(addr); newl();

void* malloc(u64 request_size) {
	/*
	 * check if there is a free page
	 * if no free page is available, allocate one
	 * once you have a free page, find an area (block) long enough to accomodate the request
	 * write info abt the size of the area etc (header). return pointer
	 * at the end / beginning of each page should be the page header
	 * 
	 * for requests longer than (PAGESIZE - HEADERSIZE), just allocate the required number of pages separately
	 *
	 * to keep track of fragmentated free memory areas we keep an (ascending) ordered list of pointers to the free memory headers
	 * keep this in a separate page (can't tell if this is a horrible idea or ok)
	 *
	 */

	void* retval;
	malloc_node_t* current_list_node = fragments_list_head, * prev_node;

	// large blocks are handled differently
	if (request_size > PAGESIZE - sizeof(page_header))
		goto large_bro;

	first_setup:
	// if there is no page at all
	if (!first_page_ptr) {

		first_page_ptr = mmap(0, MALLOC_PAGE_SIZE, PROT_RW, MAP_SHARED | MAP_ANONYMOUS, ANON_FILE, PAGES(0));
		if (first_page_ptr == MAP_FAILED) {
			first_page_ptr = nullptr;
			return MALLOC_ERR;
		}

		// debug_msg("base addr:")
		// debug_msg_addr(first_page_ptr)

		// set the page header and pointers								    next     prev
		*((page_header*)first_page_ptr) = (page_header){PAGE_TYPE_NORM, 0, nullptr, nullptr};

		current_page = first_page_ptr;
		// address of first free block in the current page. since page is uninitialised, it's just after the page header
		current_block = current_page + sizeof(page_header);
		// how many bytes we have left in the page (= PAGESIZE - bytes_occupied). we only occupied data for the header, thus that's all we need to take out
		current_page_free_bytes = PAGESIZE - sizeof(page_header);
	}

	/*
	 * at this point, the memory is layed out like this
	 * first_page_pointer		=> pointer to first byte of the header of the first page
	 * current_page				=> pointer to first byte of the header of the page we're working with
	 * current_block			=> pointer to first byte of the header of the **last block we returned** (except if we've just allocated the first page)
	 * current_page_free_bytes	=> sizeof(page_header) + sum(sizeof(block_header) + bheader.size) = PAGESIZE - (u64)(current_block + current_block.size - current_page)
	 */

	// debug_msg("!!!!!!!!!")
	// debug_msg_int(((block_header*)(current_block))->block_size)
	// debug_msg_int(current_page_free_bytes)
	// debug_msg_int(PAGESIZE - (u64)(current_block + ((block_header*)(current_block))->block_size - current_page))
	// debug_msg("!!!!!!!!!")

	// scan the fragment list for the smallest fragment that fits our requirements
	find_available_fragment:
	// if true, there are fragments: look if one of them can accomodate our request and remove it from list
	while (current_list_node) {

		// the list is in ascending order: find the first item that is greater than what we need
		if (current_list_node->addr->block_size > request_size) {
			// get address
			retval = current_list_node->addr;

			// remove
			// TODO: some easy way to reuse this memory? i don't want to lose another three days sleep
			prev_node->next = current_list_node->next;

			goto fix_block_header;
		}

		// go to next
		prev_node = current_list_node;
		current_list_node = current_list_node->next;
	}

	// debug_msg("free bytes before operations:");
	// debug_msg_int(current_page_free_bytes);
	// debug_msg("block ptr before operations:");
	// debug_msg_addr(current_block);

	// we fall through if there are no fragments. try to get one from the current open page
	no_good_fragments:
	// if a block is available in the current page
	if (request_size < current_page_free_bytes - sizeof(block_header)) {
		
		// debug_msg("previous block block size");
		// debug_msg_int(((block_header*)current_block)->block_size);

		// move current block pointer
		current_block += ((block_header*)current_block)->block_size + sizeof(block_header);
		// after this operation we don't have to change `current_page_free_bytes`: we are NOT _YET_ occupying new bytes, we're just moving the block to point to the new location
		
		// assign the first available space
		retval = current_block;

	} else { // if the page is too small, we need to open a new one
		
		// open new page
		void* next_page_ptr = mmap(0, MALLOC_PAGE_SIZE, PROT_RW, MAP_SHARED | MAP_ANONYMOUS, ANON_FILE, PAGES(0));
		if (next_page_ptr == MAP_FAILED) {
			return MALLOC_ERR;
		}

		// add the unused block in the page to the fragments list
		insert_fragment(current_block);
		
		// set the page header and pointers                                next        prev
		*((page_header*)next_page_ptr) = (page_header){PAGE_TYPE_NORM, 0, nullptr, current_page};

		// ok buddy
		((page_header*)current_page)->next_page = next_page_ptr;
		current_page = next_page_ptr;

		// address of first free block in the current page. since page is uninitialised, it's just after the page header
		current_block = current_page + sizeof(page_header);
		// how many bytes we have left in the page (= PAGESIZE - bytes_occupied). we only occupied data for the header, thus that's all we need to take out
		current_page_free_bytes = PAGESIZE - sizeof(page_header);

		retval = current_block;
	}

	// debug_msg("before block header")
	// debug_msg_int(current_page_free_bytes);
	// debug_msg_addr(current_block)

	fix_block_header:
	// write the block header
	*((block_header*)retval) = (block_header){ BLOCK_TYPE_USED, request_size };
	retval += sizeof(block_header);

	current_page_free_bytes -= request_size + sizeof(block_header);

	// debug_msg("free bytes:");
	// debug_msg_int(current_page_free_bytes);
	// debug_msg("current block:");
	// debug_msg_addr(current_block);

	// return value
	return retval;

	large_bro:
	// TODO
}

void  free(void* ptr) {
	// add a fragment based on the header
}

void* realloc(void* ptr, u64 size) {
	
}
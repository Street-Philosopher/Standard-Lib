// this is the reason i have no friends
// i'm currently sacrificing small optimisations for readability. this hurts, especially since i can't actually compile with -O >= 2 bc for whatever reason it just breaks

#include "../mem.h"
#include "../files.h"

//for debug
#include "../io.h"
#include "../color.h"

#define MALLOC_PAGE_SIZE PAGES(1)

#define PAGE_TYPE_NORM 1
#define PAGE_TYPE_HUGE 2

#define BLOCK_TYPE_FREE 0
#define BLOCK_TYPE_USED 1

#define ALLOCATE_LIST() mmap(0, MALLOC_PAGE_SIZE, PROT_RW, MAP_SHARED | MAP_ANONYMOUS, ANON_FILE, PAGES(0))
#define ALLOCATE_PAGE() mmap(0, MALLOC_PAGE_SIZE, PROT_RW, MAP_SHARED | MAP_ANONYMOUS, ANON_FILE, PAGES(0))
#define ALLOCATE_PAGE_LARGE(size) mmap(0, pagealign(size), PROT_RW, MAP_SHARED | MAP_ANONYMOUS, ANON_FILE, PAGES(0))

typedef struct malloc_page_header_s {
	u8 page_type;			// normal, huge
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

// after this it will point to the next header
#define MOVE_TO_NEXT_BLOCK_PTR() current_block += ((block_header*)current_block)->block_size + BHEADERSIZE

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


// aligns based on WORDSIZE (defined in `mem.h`)
u64 align(u64 value) {
	// i genuintely hate how stupid this is
	// at least it can only get better from here
	// or maybe i'll come up with an even worse
	while (value & (WORDSIZE-1)) {
		value++;
	}
	return value;
}
// aligns based on PAGESIZE (defnied in `mem.h`)
u64 pagealign(u64 value) {
	printhex(value);newl();
	while (value & (PAGESIZE-1)) {
		value++;
	}
	printhex(value);newl();
	return value;
}

void insert_fragment(block_header* block) {

	// TODO: if the fragment is too small ignore request
	// TODO: find the block size

	// TODO: better to ensure this is done outside this function
	// *block = (block_header) { BLOCK_TYPE_FREE, (current_page_free_bytes - BHEADERSIZE) };
	//
	// malloc_node_t* cur = fragments_list_head;
	//
	// if (cur == nullptr) {
	// 	fragments_list_head = ALLOCATE_LIST();
	// 	*fragments_list_head = (malloc_node_t){ block, nullptr };
	// 	return;
	// }
	//
	// while(cur->next) {
	// 	if (((malloc_node_t*)(cur->next))->addr->block_size > block) {
	// 		break;
	// 	}
	// 	cur = cur->next;
	// }
	//
	// new.addr = block;
	// new.next = cur->next;
	// cur->next = &new;

}
void remove_fragment(malloc_node_t* prev, u64 request_size) {

	// TODO: some easy way to reuse this memory? i don't want to lose another three days sleep
	prev->next = ((malloc_node_t*)(prev->next))->next;

	// TODO: add back the rest of the fragment if we can. if we request 8 bytes and there's a 3KB fragment we can't just give it whole and waste everything els
}

#define debug_0					print("\t" color(FORE_YELLOW) "IN MALLOC" color(FORE_WHITE) ": ")
#define debug_msg(x)			debug_0; print(x "\n");
#define debug_msg_int(i)		debug_o; printint(i); newl();
#define debug_msg_addr(addr)	debug_0; printhex(addr); newl();

void* malloc(u64 request_size) {
	/*
	 * check if there is a free page
	 * if no free page is available, allocate one
	 * once you have a free page, find an area (block) long enough to accomodate the request
	 * write info abt the size of the area etc (header). return pointer
	 * at the end / beginning of each page should be the page header
	 * 
	 * for requests longer than (PAGESIZE - (PHEADERSIZE + BHEADERSIZE)), just allocate the required number of pages separately
	 *
	 * to keep track of fragmentated free memory areas we keep an (ascending) ordered list of pointers to the free memory headers
	 * keep this in a separate page (can't tell if this is a horrible idea or ok)
	 *
	 */

	// large blocks are handled differently
	if (request_size > (PAGESIZE - (PHEADERSIZE + BHEADERSIZE)))
		goto large_bro;

	void* retval;
	malloc_node_t* current_list_node = fragments_list_head, * prev_node;

	// force alignment to word size
	request_size = align(request_size);

	first_setup:
	// if there is no page at all
	if (!first_page_ptr) {

		first_page_ptr = ALLOCATE_PAGE();
		if (first_page_ptr == MAP_FAILED) {
			first_page_ptr = nullptr;
			return MALLOC_ERR;
		}

		debug_msg_addr((u64)first_page_ptr);

		// set the page header and pointers								    next     prev
		*((page_header*)first_page_ptr) = (page_header){PAGE_TYPE_NORM, 0, nullptr, nullptr};

		current_page = first_page_ptr;
		// address of first free block in the current page. since page is uninitialised, it's just after the page header
		current_block = current_page + PHEADERSIZE;
		// how many bytes we have left in the page (= PAGESIZE - bytes_occupied). we only occupied data for the header, thus that's all we need to take out
		current_page_free_bytes = (PAGESIZE - (PHEADERSIZE + BHEADERSIZE));
	}

	/*
	 * at this point, the memory is layed out like this
	 * first_page_pointer		=> pointer to first byte of the header of the first page
	 * current_page				=> pointer to first byte of the header of the page we're working with
	 * current_block			=> pointer to first byte of the header of the **last block we returned** (except if we've just allocated the first page)
	 * current_page_free_bytes	=> PHEADERSIZE + sum(BHEADERSIZE + bheader.size) = PAGESIZE - (current_block + current_block.size - current_page)
	 */

	// scan the fragment list for the smallest fragment that fits our requirements
	find_available_fragment:
	// TODO: can probably do without prev_node like in insert_fragment
	// if true, there are fragments: look if one of them can accomodate our request and remove it from list
	while (current_list_node) {

		// the list is in ascending order: find the first item that is greater than what we need
		if (current_list_node->addr->block_size > request_size) {
			// get address
			retval = current_list_node->addr;

			// remove
			remove_fragment(prev_node, request_size);

			goto fix_block_header;
		}

		// go to next
		prev_node = current_list_node;
		current_list_node = current_list_node->next;
	}

	// we fall through if there are no fragments. try to get one from the current open page
	no_good_fragments:
	// if a block is available in the current page
	if (request_size < current_page_free_bytes - BHEADERSIZE) {
		
		// move current block pointer
		// FIXME: this wastes BHEADERSIZE bytes on the first page
		MOVE_TO_NEXT_BLOCK_PTR();
		// after this operation we don't have to change `current_page_free_bytes`: we are NOT _YET_ occupying new bytes, we're just moving the block to point to the new location
		
		// assign the first available space
		retval = current_block;

	} else { // if the page is too small, we need to open a new one
		
		// open new page
		void* next_page_ptr = ALLOCATE_PAGE();
		if (next_page_ptr == MAP_FAILED) {
			return MALLOC_ERR;
		}

		// add the unused block in the page to the fragments list
		MOVE_TO_NEXT_BLOCK_PTR();
		insert_fragment(current_block);
		
		// set the page header and pointers                                  nxt        prev
		*((page_header*)next_page_ptr) = (page_header){ PAGE_TYPE_NORM, 0, nullptr, current_page };

		// ok buddy
		((page_header*)current_page)->next_page = next_page_ptr;		// set previous page to point to this one
		current_page = next_page_ptr;

		// address of first free block in the current page. since page is uninitialised, it's just after the page header
		current_block = current_page + PHEADERSIZE;
		// how many bytes we have left in the page (= PAGESIZE - bytes_occupied). we only occupied data for the header, thus that's all we need to take out
		current_page_free_bytes = (PAGESIZE - (PHEADERSIZE + BHEADERSIZE));

		retval = current_block;
	}

	/*
	 * at this point, the memory is layed out like this
	 * current_block			=> first byte of the header of the block we're about to allocate and return
	 * current_page_free_bytes	=> not changed, unless a new page has been opened
	 * retval					=> current_block
	 */
	
	fix_block_header:
	// write the block header
	*((block_header*)retval) = (block_header){ BLOCK_TYPE_USED, request_size };
	retval += BHEADERSIZE;

	current_page_free_bytes -= request_size + BHEADERSIZE;

	// return value
	return retval;

	large_bro:

	void* next_page_ptr = ALLOCATE_PAGE_LARGE(request_size + PHEADERSIZE);

	// set the page header and pointers                                  nxt      prv
	*((page_header*)next_page_ptr) = (page_header){ PAGE_TYPE_HUGE, 0, nullptr, nullptr };
	
	return next_page_ptr + PHEADERSIZE;

}

void  free(void* ptr) {

	debug_msg("henlo")

	print("!!WARNING!!: `free` not implemented");

	ptr -= BHEADERSIZE;

	// if pointer we're freeing is the last in the page
	if (current_block == (((block_header*)ptr)->block_size + ptr)) {
		
	}

	// TODO: more in general, search if there is another free header before or after

	// add a fragment on the list based on the header
	// if the memory area is the last in the current page,just remove it as if nothing ever happened
	// set current_page to current_page->prev if done
}

void* realloc(void* ptr, u64 size) {
	
}
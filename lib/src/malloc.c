// this is the reason i have no friends

#include "../mem.h"
#include "../files.h"

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

// points to the first byte of the header.
// we almost never reference first_page, all we care about is wether there's
// space in the current (other pages are referenced via fragments)
void* first_page_ptr = nullptr;

// list of all memory areas that have been `free`ed
malloc_node_t* fragments_list_head = nullptr;

// point to first byte of respective headers
void* current_page = nullptr;
void* current_block = nullptr;		//  \___ keep both for ease of calculation
s16 current_page_free_bytes;		//  /


void insert_fragment(block_header* block) {
	*block = (block_header) { BLOCK_TYPE_FREE, (current_page_free_bytes - sizeof(block_header)) };

	malloc_node_t* 	cur  = fragments_list_head,
					new;							// TODO: oh fuck, this is in the stack. problem for future me

	while(cur->next) {
		if (((malloc_node_t*)(cur->next))->addr->block_size > block) {
			break;
		}
	}

	new.addr = block;
	new.next = cur->next;
	cur->next = &new;

}

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
	malloc_node_t* current_list_node = fragments_list_head;

	// large blocks are handled differently
	if (request_size > PAGESIZE - sizeof(page_header))
		goto large_bro;

	first_setup:
	// if there is no page at all
	if (!first_page_ptr) {
		first_page_ptr = mmap(0, PAGES(1), PROT_RW, MAP_SHARED | MAP_ANONYMOUS, ANON_FILE, PAGES(0));
		if (first_page_ptr == MAP_FAILED) {
			first_page_ptr = nullptr;
			return MALLOC_ERR;
		}
		
		// set the page header and pointers								    next     prev
		*((page_header*)first_page_ptr) = (page_header){PAGE_TYPE_NORM, 0, nullptr, nullptr};

		current_page = first_page_ptr;
		// address of first free block in the current page. since page is uninitialised, it's just after the page header
		current_block = current_page + sizeof(page_header);
		// how many bytes we have left in the page (= PAGESIZE - bytes_occupied). we only occupied data for the header, thus that's all we need to take out
		current_page_free_bytes = PAGESIZE - sizeof(page_header);
	}

	// scan the first page for free blocks, if there aren't go to the next page and so forth
	find_available_block:
	// there are fragments: look if one of them can accomodate our request
	// find smallest fragment that fits our requirements and remove it from list
	while (current_list_node) {
		// the list is ordered: find the first item that is greater than what we need
		if (current_list_node->addr->block_size > request_size) {
			// get address
			retval = current_list_node->addr;

			// TODO: remove

			goto fix_block_header;
		}

		// go to next
		current_list_node = current_list_node->next;
	}

	// we fall through if there are no fragments. try to get one from the current open page
	no_good_fragments:
	// if a block is available in the current page
	if (request_size < current_page_free_bytes - sizeof(block_header)) {

		// assign the first available space
		retval = current_block;

	} else { // if the page is too small, we need to open a new one
		// add the unused block in the page to the fragments list
		//TODO: should be done later, to account for the possibility of mmap failing
		insert_fragment(current_block);

		// open new page
		void* next_page_ptr = mmap(0, PAGES(1), PROT_RW, MAP_SHARED | MAP_ANONYMOUS, ANON_FILE, PAGES(0));
		if (next_page_ptr == MAP_FAILED) {
			next_page_ptr = nullptr;
			return MALLOC_ERR;
		}
		
		// set the page header and pointers                                next        prev
		*((page_header*)next_page_ptr) = (page_header){PAGE_TYPE_NORM, 0, nullptr, current_page};

		// ok buddy
		((page_header*)current_page)->next_page = next_page_ptr;
		current_page = next_page_ptr;

		// address of first free block in the current page. since page is uninitialised, it's just after the page header
		current_block = current_page + sizeof(page_header);
		// how many bytes we have left in the page (= PAGESIZE - bytes_occupied). we only occupied data for the header, thus that's all we need to take out
		current_page_free_bytes = PAGESIZE - sizeof(page_header);
	}

	fix_block_header:
	// write the block header
	*((block_header*)retval) = (block_header){ BLOCK_TYPE_USED, request_size };
	retval += sizeof(block_header);

	// return value
	return retval;

	large_bro:
	// TODO
}

void  free(void* ptr) {

}

void* realloc(void* ptr, u64 size) {
	
}
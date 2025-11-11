// this is the reason i have no friends

#include "../mem.h"
#include "../files.h"

typedef struct malloc_page_header_s {
	u8 page_type;			// normal, long
	fd_t file;				// file descriptor for mapping
	u16 usages;				// number of times malloc has been called on this page
	u64 next_page;
	u64 prev_page;
} page_header;
typedef struct malloc_block_header_s {
	u8 block_type;			// free, used
	u64 block_size;
	u64 prev_block;
} block_header;

// 
void* first_page_ptr = nullptr;

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
void* malloc(u64 size) {
	// 	for small blocks:
	// 		if no page is allocated, do so
	// 		scan the first page for free blocks, if there aren't go to the next page and so forth
	//		write the block header and return

	// 	for large blocks:
	// 		allocate
}

void  free(void* ptr) {

}

void* realloc(void* ptr, u64 size) {
	
}
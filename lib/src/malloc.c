// this is the reason i have no friends
// i'm currently sacrificing small optimisations for readability. this hurts, especially since i can't actually compile with -O >= 2 bc for whatever reason it just breaks

#include "../mem.h"
#include "../files.h"

//for debug
#include "../io.h"
#include "../color.h"

// TODO: find and substitute
#define MALLOC_PAGE_SIZE PAGES(1)

enum pagetype {
	PAGE_TYPE_NULL,
	PAGE_TYPE_NORM,
	PAGE_TYPE_HUGE,

	PAGE_TYPE_MAX
};
enum blocktype {
	BLOCK_TYPE_FREE,
	BLOCK_TYPE_USED,
	BLOCK_TYPE_HUGE,

	BLOCK_TYPE_MAX
};

#define ALLOCATE_PAGE() mmap(0, MALLOC_PAGE_SIZE, PROT_RW, MAP_SHARED | MAP_ANONYMOUS, ANON_FILE, PAGES(0))
#define ALLOCATE_PAGE_LARGE(size) mmap(0, (size), PROT_RW, MAP_SHARED | MAP_ANONYMOUS, ANON_FILE, PAGES(0))
#define ALLOCATE_FRAGMENT_PAGE() mmap(0, MALLOC_PAGE_SIZE, PROT_RW, MAP_SHARED | MAP_ANONYMOUS, ANON_FILE, PAGES(0))

typedef struct malloc_page_header_s {
	u8 page_type;			// normal, huge
	u64 usages;				// number of times malloc has been called on this page
	void* next_page;
	void* prev_page;
} page_header;
typedef struct malloc_block_header_s {
	u8 block_type;			// free, used, large
	u64 block_size;
} block_header;

typedef struct node_page_header_s {
	u64 free_spaces;
	void* next_page;
} malloc_node_header;
typedef struct malloc_node_s {
	block_header* addr;		// address of the block's header
	void* next;				// next node on list
} malloc_node_t;

#define PHEADERSIZE sizeof(page_header)
#define BHEADERSIZE sizeof(block_header)
#define NODESIZE	sizeof(malloc_node_t)
#define NHEADERSIZE sizeof(malloc_node_header)

// amount of nodes we can define in a single fragment page
#define max_list_nodes_perç_page ((PAGESIZE - NHEADERSIZE) / NODESIZE)

// after this it will point to the next header
#define MOVE_TO_NEXT_BLOCK_PTR() current_block += ((block_header*)current_block)->block_size + BHEADERSIZE

// points to the first byte of the header.
// we almost never reference first_page, all we care about is wether there's
// space in the current (other pages are referenced via fragments)
void* first_page_ptr = nullptr;

// list of all memory areas that have been `free`ed
// this points to the HEADER of the FIRST PAGE of fragments
malloc_node_t* fragments_list_head = nullptr;					// TODO: change this type

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
	while (value & (PAGESIZE-1)) {
		value++;
	}
	return value;
}

void insert_fragment(block_header* block) {

	/*
	 * check header of all fragment pages
	 * if we can find one with free space, add the fragment to that space and return
	 * else allocate a new fragment page and add the fragment there
	 */

	// TODO:
	// IMPORTANT: ensure the block's size is set before calling this function. the function WILL assume you've done so

	ASSERT(NHEADERSIZE == 16)
	ASSERT(NODESIZE    == 16)

	block->block_type = BLOCK_TYPE_FREE;

	// if the fragment is too small ignore request
	if (block->block_size == 0) {	// it is a possibility
		return;
	}

	malloc_node_t* cur = fragments_list_head;
	malloc_node_t* new;
	
	if (cur == nullptr) {
		fragments_list_head = ALLOCATE_FRAGMENT_PAGE();
		((malloc_node_header*)fragments_list_head)->free_spaces = max_list_nodes_perç_page;
		*(fragments_list_head + NHEADERSIZE) = (malloc_node_t){ block, nullptr };
		return;
	}

	// find first fragment page with a free block
	// this section sets `new` to the first empty section we can find, or allocates a new fragment page if none could be found
	SECTION(find_space) {
		while((malloc_node_header*)cur) {
			// if there is at least one free space
			if (((malloc_node_header*)cur)->free_spaces) {
				// we don't care what it is, but for the next scan we'll have to notice that we've taken a space
				((malloc_node_header*)cur)->free_spaces--;
				goto page_found;
			}
			cur = cur->next;
		}
		// fall through
		no_page_found:

		new = ALLOCATE_FRAGMENT_PAGE();
		*((malloc_node_header*)new) = (malloc_node_header){ max_list_nodes_perç_page, nullptr };
		((malloc_node_header*)cur)->next_page = new;
		new += NHEADERSIZE;

		// go to next section
		goto insert_block;

		// found a fragment page with free space! let's find the space in question and set `new` to it
		page_found:
		// cur now contains the fragment page we're interested in. we're going to scan it all and find the first empty node (block_header* == 0)
		cur += NHEADERSIZE; // ignore header
		for (int c = 0; c < max_list_nodes_perç_page; c++ /*c++ lol imagine*/) {
			if (!cur->addr) {
				new = cur;
				break;
			}

			cur += NODESIZE;
		}

		// fall through to next section
	}
	
	// find where to insert the block
	// this section sets `cur` to the first block with size greater than our request
	SECTION(insert_block) {
		cur = fragments_list_head + NHEADERSIZE;
		if (!cur->next) {
			// TODO: what happens if cur is the only node in the list?
		}
		while(cur->next) {
			if (((malloc_node_t*)(cur->next))->addr->block_size > block) {
				break;
			}
			cur = cur->next;
		}
	}
	
	// insert node in list
	new->addr = block;
	new->next = cur->next;
	cur->next = new;

	return;

}
void remove_fragment(malloc_node_t* prev, u64 request_size) {

	/*
	 * update adiacent list nodes
	 * update information in the location's fragment page's header
	 * request_size means: if the size of the fragment we're going to remove is smaller than request_size resize the fragment rather than remove it
	 */

	// save for later use
	malloc_node_t* cur = prev->next;

	// update list
	prev->next = ((malloc_node_t*)(prev->next))->next;

	// set block_header addr to zero for insert_fragment
	cur->addr = 0;

	// update info in the header
	SECTION(fix_header) {
		// first we scan all pages, and find where `cur` lives
		malloc_node_header* cp = fragments_list_head;
		do {
			if (cp < cur && cur < cp + PAGESIZE) {
				break;
			}
			cp = cp->next_page;
		} while (cp);		// this condition should NEVER be false

		// then we use this information
		cp->free_spaces++;
	}

	// TODO: add back the rest of the fragment if we can. if we request 8 bytes and there's a 3KB fragment we can't just give it whole and waste everything els
}

#define debug_0					print("\t" color(FORE_BLUE) /* color(BACK_WHITE) */ "IN MALLOC" color(COLOR_RESET_ALL) ": ")
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

		// TODO: should maybe initialise all memory to zero bc of header checks

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
	 * current_page_free_bytes	=> PHEADERSIZE + sum(BHEADERSIZE + bheader.size) = PAGESIZE - ((current_block - current_page) + current_block.size)
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
		// if the block is unused we don't need to
		if (((block_header*)current_block)->block_type /* != BLOCK_TYPE_FREE */) {
			MOVE_TO_NEXT_BLOCK_PTR();
		}
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
		// set correct current free bytes
		((block_header*)current_block)->block_size = MALLOC_PAGE_SIZE - (current_block - current_page /*offset*/) - BHEADERSIZE;
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

	// large blocks are handled separately
	large_bro:
	
	// TODO: some testing but `pagealign`ing doesn't seem to be needed as mmap already does it (allegedly)
	request_size = pagealign(request_size + BHEADERSIZE);
	
	// just allocate a number of pages solely for this thing
	void* next_page_ptr = ALLOCATE_PAGE_LARGE(request_size);
	
	// because of how free is handled, we're going to use a block header instead of a page header
	*((block_header*)next_page_ptr) = (block_header){ BLOCK_TYPE_HUGE, request_size };
	
	return next_page_ptr + BHEADERSIZE;

}

void  free(void* ptr) {

	print(color(FORE_RED) "!!WARNING!!" color(FORE_WHITE) ": `free` not implemented\n");

	ptr -= BHEADERSIZE;

	if (((block_header*)ptr)->block_type == BLOCK_TYPE_HUGE)
		goto large_free_bro;

	// if pointer we're freeing is the last in the page
	if (current_block == (((block_header*)ptr)->block_size + ptr)) {
		
	}

	// TODO: more in general, search if there is another free header before or after

	// add a fragment on the list based on the header
	// if the memory area is the last in the current page,just remove it as if nothing ever happened
	// set current_page to current_page->prev if done

	large_free_bro:

}

void* realloc(void* ptr, u64 size) {
	
}
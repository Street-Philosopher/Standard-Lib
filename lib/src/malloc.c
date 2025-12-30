// this is the reason i have no friends
// i'm currently sacrificing small optimisations for readability. this hurts, especially since i can't actually compile with -O >= 2 bc for whatever reason it just breaks

#include "../mem.h"
#include "../util.h"		// for ASSERT and SECTION
#include "../error.h"

#include "../debug.h"

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

struct malloc_page_header_s;
struct malloc_block_header_s;
typedef struct malloc_page_header_s page_header;
typedef struct malloc_block_header_s block_header;

struct malloc_page_header_s {
	u64 page_type;			// normal, huge
	u64 usages;				// number of times malloc has been called on this page
	page_header* next_page;
	page_header* prev_page;
};
struct malloc_block_header_s {
	u64 block_type;			// free, used, large
	u64 block_size;
	u64 page;				// used by free to update page stats
};

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
#define max_list_nodes_perç_page ((MALLOC_PAGE_SIZE - NHEADERSIZE) / NODESIZE)

// after this it will point to the next header
#define MOVE_TO_NEXT_BLOCK_PTR() 															\
		/* if the block is unused we don't need to */										\
		if (((block_header*)current_block)->block_type /* != BLOCK_TYPE_FREE */) { 			\
			current_block += ((block_header*)current_block)->block_size + BHEADERSIZE;		\
		}

// points to the first byte of the header.
// we almost never reference first_page, all we care about is wether there's
// space in the current (other pages are referenced via fragments)
page_header* first_page_ptr = nullptr;

// list of all memory areas that have been `free`ed
// this points to the HEADER of the FIRST PAGE of fragments
malloc_node_header* first_fragment_list_header = nullptr;
// this points to the FIRST NODE in the list. keep both for convenience sake
malloc_node_t* fragments_list_head = nullptr;

// point to first byte of respective headers
void* current_page = nullptr;
void* current_block = nullptr;		//  \___ keep both for ease of calculation
s64 current_page_free_bytes = 0;	//  /					// TODO: i'm pretty sure since we have page.usages this is unnecessary 


// aligns based on WORDSIZE (defined in `mem.h`)
u64 align(u64 value);
void _() { asm(
	".text"												"\n\t"
	".globl	align"										"\n\t"
	".type	align, @function"							"\n"
"align:"												"\n\t"
	"mov rax,rdi"										"\n\t"
	"add rax,   %0 - 1"									"\n\t"
	"and rax, ~(%0 - 1)"								"\n\t"
	"ret"												"\n\t"
	".size	align, .-align"								"\n\t"
	".globl	insert_fragment"							"\n\t"
	".type	insert_fragment, @function"					"\n\t"

	:: 	"i"(WORDSIZE)
); }

// bitmasks? hell nah
u64 insert_fragment(block_header* block) {

	/*
	 * check header of all fragment pages
	 * if we can find one with free space, add the fragment to that space and return
	 * else allocate a new fragment page and add the fragment there
	 * 
	 * returns zero on success, errcode otherwise
	 */

	// IMPORTANT: ensure the block's size is set before calling this function. the function WILL assume you've done so

	// we make the assumption that we can fit nodes in a fragment page without any leftover space, and thus this is important
	ASSERT(NHEADERSIZE == 16)
	ASSERT(NODESIZE    == 16)

	// we need it in the MOVE_TO_NEXT_BLOCK_PTR() macro
	ASSERT(BLOCK_TYPE_FREE == 0)

	// if the fragment is too small ignore request
	if (block->block_size == 0) {	// it is a possibility
		return 0;
	}

	malloc_node_t* cur = (malloc_node_t*)first_fragment_list_header;
	malloc_node_t* new;
	
	if (cur == nullptr) {
		first_fragment_list_header = ALLOCATE_FRAGMENT_PAGE();
		if (iserr(first_fragment_list_header)) {
			return (u64)first_fragment_list_header;
		}

		fragments_list_head = (void*)first_fragment_list_header + NHEADERSIZE;

		first_fragment_list_header->free_spaces = (max_list_nodes_perç_page);
		first_fragment_list_header->next_page   = (nullptr);
		
		block->block_type = BLOCK_TYPE_FREE;
		*fragments_list_head = (malloc_node_t){ block, nullptr };
		return 0;
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
		if (iserr(new)) {
			return (u64)new;
		}

		*((malloc_node_header*)new) = (malloc_node_header){ max_list_nodes_perç_page, nullptr };
		((malloc_node_header*)cur)->next_page = new;
		new = (void*)new + NHEADERSIZE;

		// go to next section
		goto insert_block;

		// found a fragment page with free space! let's find the space in question and set `new` to it
		page_found:
		// cur now contains the fragment page we're interested in. we're going to scan it all and find the first empty node (block_header* == 0)
		cur = (void*)cur + NHEADERSIZE; // ignore header
		for (int c = 0; c < max_list_nodes_perç_page; c++ /*c++ lol imagine*/) {
			if (!cur->addr) {
				new = cur;
				break;
			}

			cur = (void*)cur + NODESIZE;
		}
		// there shouldn't be a fall through from the loop, as we know there is at least one free fragment in the page

		// fall through to next section
	}
	
	// find where to insert the block
	// this section sets `cur` to the first block with size greater than our request
	SECTION(insert_block) {
		cur = fragments_list_head;
		// if cur is the only node in the list, the program would just insert the new node after cur,
		// regardless of its block size. thus, if the node's size is less than cur's we handle here
		// TODO: any way to condense this?
		if (!cur->next && cur->addr->block_size) {
			// insert node before cur
			new->addr = block;
			fragments_list_head = new;
			new->next = cur;
		}
		while(cur->next) {
			if (((malloc_node_t*)(cur->next))->addr->block_size >= block->block_size) {
				break;
			}
			cur = cur->next;
		}
	}

	block->block_type = BLOCK_TYPE_FREE;
	
	// insert node in list
	new->addr = block;
	new->next = cur->next;
	cur->next = new;

	return 0;

}
u64 remove_fragment(malloc_node_t* prev, u64 request_size) {

	/*
	 * update adiacent list nodes
	 * update information in the location's fragment page's header
	 * request_size means: if the size of the fragment we're going to remove is smaller than request_size resize the fragment rather than remove it
	 * 
	 * returns zero on success, errcode otherwise
	 */

	malloc_node_t* cur;

	// IMPORTANT: special edge case if the node is found on the first try by the outer loop
	if (prev == nullptr) {
		// prev == nullptr ==> current node is the first one
		cur = fragments_list_head;

		debug_msg("node was found on first try")
		debug_msg_addr(cur)

		// if the first element was also the last element in fragments list, deallocate
		if (!cur->next) {
			debug_msg("deallocating")
			u64 res = munmap((u64)first_fragment_list_header, MALLOC_PAGE_SIZE);
			if (iserr(res)) {
				return res;
			}

			fragments_list_head = nullptr;
			first_fragment_list_header = nullptr;
			return 0;
		}

		// remove first element in list
		cur = fragments_list_head;
		fragments_list_head = cur->next;

	} else {
		// save for later use
		cur = prev->next;

		// update list
		if (prev->next)		// FIXME: is this correct??
			prev->next = ((malloc_node_t*)(prev->next))->next;
	}


	// set block_header addr to zero for insert_fragment
	// FIXME: this segfaults if prev is the last node in the list. can prev be the last node in the list?
	cur->addr = 0;

	// update info in the header
	SECTION(fix_header) {
		// first we scan all pages, and find where `cur` lives
		malloc_node_header* cp = first_fragment_list_header, * prev_page;
		do {
			if ((u64)cp < (u64)cur && (u64)cur < ((u64)cp) + MALLOC_PAGE_SIZE) {
				break;
			}
			cp = cp->next_page;
			prev_page = cp;
		} while (cp);		// this condition should NEVER be false, as the cur is by definition within the cp

		// then we use this information
		cp->free_spaces++;

		// deallocate pages when header is bad
		if (cp->free_spaces == max_list_nodes_perç_page) {
			// FIXME: changes have been made to our structures before this statement
			//			+ increased cp->free_spaces
			//			+ updated fragment list
			// thing is munmap should never fail. but what if?
			u64 res = munmap((u64)cp, MALLOC_PAGE_SIZE);
			if (iserr(res)) {
				// print(color(FORE_RED) "CATASTROPHIC FAILURE" color(COLOR_RESET_ALL));
				PANIC();
				return res;
			}

			// update page list
			if (cp == first_fragment_list_header) {
				first_fragment_list_header = cp->next_page;
			} else {
				prev_page->next_page = cp->next_page;
			}
		}
	}

	// TODO: add back the rest of the fragment if we can. if we request 8 bytes and there's a 3KB fragment we can't just give it whole and waste everything els
	
	return 0;
}

void* malloc(u64 request_size) {

	debug_msg("requested")
	debug_msg_int(request_size)
	
	/*
	 * check if there is a free page
	 * if no free page is available, allocate one
	 * once you have a free page, find an area (block) long enough to accomodate the request
	 * write info abt the size of the area etc (header). return pointer
	 * at the end / beginning of each page should be the page header
	 * 
	 * for requests longer than (MALLOC_PAGE_SIZE - (PHEADERSIZE + BHEADERSIZE)), just allocate the required number of pages separately
	 *
	 * to keep track of fragmentated free memory areas we keep an (ascending) ordered list of pointers to the free memory headers
	 * keep this in a separate page (can't tell if this is a horrible idea or ok)
	 *
	 */

	ASSERT(iserr(MALLOC_ERR))
	if (request_size < 1) return MALLOC_ERR;

	// large blocks are handled differently
	if (request_size > (MALLOC_PAGE_SIZE - (PHEADERSIZE + BHEADERSIZE)))
		goto large_bro;

	ASSERT(sizeof(void*) == sizeof(u64))
	u64 tmp;
	void* retval;
	malloc_node_t* current_list_node = fragments_list_head, * prev_node;

	// force alignment to word size
	request_size = align(request_size);
	debug_msg("aligned size:")
	debug_msg_int(request_size)

	first_setup:
	// if there is no page at all
	if (!first_page_ptr) {

		first_page_ptr = ALLOCATE_PAGE();
		debug_msg("first_page_ptr:")
		debug_msg_addr(first_page_ptr)
		if (iserr(first_page_ptr)) {
			debug_msg("error: ")
			debug_msg(geterrname((u64)first_page_ptr))
			tmp = (u64)first_page_ptr;
			first_page_ptr = nullptr;
			return (void*)tmp;
		}
		
		// a moment of silence for uninitialised memory
		memset(first_page_ptr, 0, MALLOC_PAGE_SIZE);

		// set the page header and pointers								usages    next     prev
		*first_page_ptr = (page_header){PAGE_TYPE_NORM, 0,       nullptr, nullptr};

		current_page = first_page_ptr;
		// address of first free block in the current page. since page is uninitialised, it's just after the page header
		current_block = current_page + PHEADERSIZE;
		// how many bytes we have left in the page (= MALLOC_PAGE_SIZE - bytes_occupied). we only occupied data for the header, thus that's all we need to take out
		current_page_free_bytes = (MALLOC_PAGE_SIZE - (PHEADERSIZE + BHEADERSIZE));

		debug_msg("end of first page set up")
	}

	/*
	 * at this point, the memory is layed out like this
	 * first_page_pointer		=> pointer to first byte of the header of the first page
	 * current_page				=> pointer to first byte of the header of the page we're working with
	 * current_block			=> pointer to first byte of the header of the **last block we returned** (except if we've just allocated the first page)
	 * current_page_free_bytes	=> PHEADERSIZE + sum(BHEADERSIZE + bheader.size) = MALLOC_PAGE_SIZE - ((current_block - current_page) + current_block.size)
	 */

	// scan the fragment list for the smallest fragment that fits our requirements
	find_available_fragment:
	// prev_node == nullptr will signal we found our node on the first try, which triggers a special case inside remove_fragment 
	prev_node = nullptr;

	// FIXME: this never accesses the last node in the fragment list
	// if true, there are fragments: look if one of them can accomodate our request and remove it from list
	while (current_list_node) {
		// the list is in ascending order: find the first item that is greater than what we need
		if (current_list_node->addr->block_size >= request_size) {
			// get address
			retval = current_list_node->addr;
			debug_msg("found frgment");
			debug_msg_addr(retval);

			// remove
			if (tmp = remove_fragment(prev_node, request_size)) {
				/* return error code */
				debug_err("failed to remove fragment:")
				debug_msg_addr(prev_node)
				debug_msg_int(request_size)

				// no changes have been made, except for allocating the first page in case this hadn't been done already
				// in that case, we can just keep it as an empty page
				// remove fragment already made sure no changes had been made
				// we can just return the err code
				return (void*)tmp;
			}

			goto fix_block_header;
		}

		// go to next
		prev_node = current_list_node;
		current_list_node = current_list_node->next;
	}

	// we fall through if there are no fragments. try to get one from the current open page
	no_good_fragments:
	debug_msg("no good fragments")
	// if a block is available in the current page
	if (request_size <= current_page_free_bytes - BHEADERSIZE) {

		debug_msg("block available")
		
		// move current block pointer
		MOVE_TO_NEXT_BLOCK_PTR();
		// after this operation we don't have to change `current_page_free_bytes`: we are NOT _YET_ occupying new bytes, we're just moving the block to point to the new location
		
		// assign the first available space
		retval = current_block;

	} else { // if the page is too small, we need to open a new one
		
		debug_msg("open new page")

		// open new page
		page_header* next_page_ptr = ALLOCATE_PAGE();
		if (iserr(next_page_ptr)) {
			return next_page_ptr;
		}

		memset(next_page_ptr, 0, MALLOC_PAGE_SIZE);

		// add the unused block in the page to the fragments list
		MOVE_TO_NEXT_BLOCK_PTR();
		// set correct current free bytes
		((block_header*)current_block)->block_size = MALLOC_PAGE_SIZE - (current_block - current_page /*offset*/) - BHEADERSIZE;
		if (tmp = insert_fragment(current_block)) {
			/* return error code */
			debug_msg("error in insert fragment:")
			debug_msg_addr(current_block)

			// if we reach here, all we did was allocate a page that we haven't used yet and move to the next block on the current page

			// munmap the unused page so we can still use the block we failed to fragment
			munmap((u64)next_page_ptr, MALLOC_PAGE_SIZE);

			// we already point to the block we failed to fragment, thus on the next malloc call we'll try to reuse it
			return (void*)tmp;
		}
		
		// set the page header and pointers                             usages    nxt        prev
		*next_page_ptr = (page_header){ PAGE_TYPE_NORM, 0,      nullptr, current_page };

		// ok buddy
		((page_header*)current_page)->next_page = next_page_ptr;		// set previous page to point to this one
		current_page = next_page_ptr;

		// address of first free block in the current page. since page is uninitialised, it's just after the page header
		current_block = current_page + PHEADERSIZE;
		// how many bytes we have left in the page (= MALLOC_PAGE_SIZE - bytes_occupied). we only occupied data for the header, thus that's all we need to take out
		current_page_free_bytes = (MALLOC_PAGE_SIZE - (PHEADERSIZE + BHEADERSIZE));

		retval = current_block;
	}

	/*
	 * at this point, the memory is layed out like this
	 * current_block			=> first byte of the header of the block we're about to allocate and return
	 * current_page_free_bytes	=> not changed, unless a new page has been opened
	 * retval					=> current_block
	 */
	
	fix_block_header:
	debug_msg("fix block header")
	debug_msg_addr(retval)
	// write the block header
	*((block_header*)retval) = (block_header){ BLOCK_TYPE_USED, request_size, (u64)current_page };
	
	// TODO: this is so ugly without actual pointers ffs
	((page_header*)(((block_header*)retval)->page))->usages++;
	current_page_free_bytes -= request_size + BHEADERSIZE;
	
	// return value
	retval += BHEADERSIZE;
	return retval;

	// large blocks are handled separately
	large_bro:
	
	// `pagealign`ing doesn't seem to be needed as mmap already does it (hopefully)
	// request_size = pagealign(request_size + BHEADERSIZE);
	
	// just allocate a number of pages solely for this thing
	void* next_page_ptr = ALLOCATE_PAGE_LARGE(request_size);
	if (iserr(next_page_ptr)) {
		return next_page_ptr;
	}
	
	// because of how free is handled, we're going to use a block header instead of a page header
	*((block_header*)next_page_ptr) = (block_header){ BLOCK_TYPE_HUGE, request_size, (u64)next_page_ptr };
	
	return next_page_ptr + BHEADERSIZE;

}

int free(void* ptr) {

	u64 tmp;

	ptr -= BHEADERSIZE;

	if (((block_header*)ptr)->block_type == BLOCK_TYPE_HUGE)
		goto large_free_bro;

	// TODO:	search if there is another free header before or after so we don't needlessly fragment memory
	// 			if the memory area is the last in the current page,just remove it as if nothing ever happened

	// add a fragment on the list based on the header
	if (tmp = insert_fragment(ptr)) {
		/* return error code */
		// we haven't really done anything at all
		return tmp;
	}

	// get current page
	page_header* block_page = (page_header*)((block_header*)ptr)->page;

	block_page->usages--;
	
	// if this page is empty munmap and update the page list
	if (block_page->usages == 0) {
		ASSERT(MALLOC_PAGE_SIZE >= 3);
		return munmap((u64)current_page, 3);
	}

	return 0;

	large_free_bro:

	return munmap((u64)ptr, 1);

}

void* realloc(void* ptr, u64 size) {
	
}
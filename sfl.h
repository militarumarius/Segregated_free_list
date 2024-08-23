#include <stddef.h>
#define LG_MAX 256

typedef struct node_t {
	struct node_t *prev, *next;
	void *data;
	int bytes;
	size_t address;

} node_t;

typedef struct list_t {
	node_t *head;
	unsigned int size;
	unsigned int data_size;
	unsigned int b_size;

} list_t;

typedef struct sfl_t {
	list_t **array_list;
	int n_malloc;
	int n_fragmentation;
	int n_free;
	int bytes_allocated;
	int nr_list;
	int bytes;
	int total_mem;
	int type;
	size_t start_address;

} sfl_t;

typedef struct {
	char sentence[LG_MAX];
	char words[LG_MAX][LG_MAX];
} words_t;

void add_nth_node(list_t *list, unsigned int n, size_t address, int bytes);
list_t *create_list(unsigned int bytes_size);
node_t *get_nth_node(list_t *list, unsigned int n);
node_t *remove_nth_node(list_t *list, unsigned int n);
void add_node_by_address(list_t *list, node_t *node, int bytes);
int check_list_size(sfl_t *h, int size);
void realloc_array_list(sfl_t *h, int crt, int type, int size);
void my_malloc(sfl_t *h, list_t *a_list);
void init_heap(sfl_t *h, list_t *alloc);
void add_node_to_heap(sfl_t *h, node_t *r);
void free_basic(sfl_t *h, list_t *alloc);
int check_initial_block_from_heap(sfl_t *h, node_t *n1, node_t *n2);
void remake_initial_block_to_right(sfl_t *h, size_t addr, node_t *rm);
void remake_initial_block_to_left(sfl_t *h, size_t addr, node_t *rm);
void free_bonus(sfl_t *h, list_t *alloc);
void free_list_heap(sfl_t *h, list_t *alloc);
void ll_free(list_t **pp_list);
void print_lists(sfl_t *h);
void dump_memory(sfl_t *h, list_t *alloc);
void destroy_heap(sfl_t *h, list_t *alloc, int *loop);
int find_words(words_t *w);
int cifreint(char c[]);
int check_write(list_t *alloc, size_t addr, int nr_bytes_left);
void seg_fault(sfl_t *h, list_t *alloc, int *loop);
void write(sfl_t *h, list_t *alloc, int *loop);
int check_read(list_t *alloc, size_t addr, int nr_bytes_left);
void read(sfl_t *h, list_t *alloc, int *loop);

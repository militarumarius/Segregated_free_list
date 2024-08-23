// Copyright Militaru-Ionut-Marius 313CAb 2023-2024

#include "sfl.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// functia adauga intr-o lista pe pozitia n un nod
void add_nth_node(list_t *list, unsigned int n, size_t address, int bytes)
{
	node_t *node = malloc(sizeof(node_t));
	node->data = NULL;
	node->address = address;
	node->bytes = bytes;
	if (n > list->size)
		n = list->size;
	if (!list->head) {
		list->head = node;
		node->next = NULL;
		node->prev = NULL;
		list->size++;
		return;
	}
	if (!n) {
		list->head->prev = node;
		node->next = list->head;
		list->head = node;
		node->prev = NULL;
		list->size++;
		return;
	}
	node_t *p = list->head;
	for (int i = 0; i < n - 1; i++)
		p = p->next;
	if (p->next)
		p->next->prev = node;
	node->next = p->next;
	node->prev = p;
	p->next = node;
	list->size++;
}

// functia creeaza o lista
list_t *create_list(unsigned int bytes_size)
{
	list_t *list = malloc(sizeof(list_t));
	list->head = NULL;
	list->b_size = bytes_size;
	list->size = 0;
	return list;
}

// functia obtine un nod de pe pozitia n dintr-o lista
node_t *get_nth_node(list_t *list, unsigned int n)
{
	node_t *curent = list->head;
	if (n >= list->size)
		n %= list->size;
	for (int i = 0; i < n; i++)
		curent = curent->next;
	return curent;
}

// functia elimina nodul de pe pozitia n din lista data
node_t *remove_nth_node(list_t *list, unsigned int n)
{
	node_t *rem;
	if (n >= list->size)
		n = list->size - 1;
	node_t *p = list->head;
	if (n == 0) {
		if (list->head->next)
			list->head->next->prev = NULL;
		list->head = p->next;
		list->size--;
		return p;
	}
	for (int i = 0; i < n - 1; i++)
		p = p->next;
	rem = p->next;
	if (n != list->size - 1)
		p->next->next->prev = p;
	p->next = p->next->next;
	rem->next = NULL;
	rem->prev = NULL;
	list->size--;
	return rem;
}

// functia adauga un nod intr-o lista in functie de adresa acestuia
// am folosit-o ca sa adaug in lista de memorie alocata si in vectorul de liste
// in ordinea crescatoare a adreselor
void add_node_by_address(list_t *list, node_t *node, int bytes)
{
	node_t *curr;
	int crt = 0;
	curr = list->head;
	while (curr) {
		if (curr->address > node->address)
			break;
		curr = curr->next;
		crt++;
	}
	add_nth_node(list, crt, node->address, bytes);
}

// functia verifica daca in vectorul de liste exista dimensiunea data
int check_list_size(sfl_t *h, int size)
{
	for (int i = 0; i < h->nr_list; i++)
		if (h->array_list[i]->b_size == size)
			return 1;
	return 0;
}

// daca type = 0 elimin un element din vectorul de liste
// daca type = 1 adaug un element in vectorul de liste
void realloc_array_list(sfl_t *h, int crt, int type, int size)
{
	if (type == 0) {
		// sterg lista curenta si realoc vectorul de liste
		free(h->array_list[crt]);
		for (int i = crt; i < h->nr_list - 1; i++)
			h->array_list[i] = h->array_list[i + 1];
		h->nr_list--;
		h->array_list = realloc(h->array_list, h->nr_list * sizeof(list_t *));
	} else {
		// adaug o noua lista si realoc vectorul de liste
		h->nr_list++;
		h->array_list = realloc(h->array_list, h->nr_list * sizeof(list_t *));
		h->array_list[h->nr_list - 1] = create_list(size);
		list_t *aux = h->array_list[h->nr_list - 1];
		for (int i = h->nr_list - 1; i > crt; i--)
			h->array_list[i] = h->array_list[i - 1];
		h->array_list[crt] = aux;
	}
}

// functie folosita petru a aloca memorie si a muta un bloc din vectorul de
// liste in lista de memeorie alocata
void my_malloc(sfl_t *h, list_t *a_list)
{
	int a_size;
	scanf("%d", &a_size);
	int i = 0;
	// gasesc lista din vectorul de liste de unde trebuie alocata memorie
	while (i < h->nr_list && a_size > h->array_list[i]->b_size)
		i++;

	if (i >= h->nr_list) {
		printf("Out of memory\n");
		return;
	}
	h->n_malloc++;
	h->bytes_allocated += a_size;
	//  elimin ultimul nod din lista libera din heap
	node_t *r = remove_nth_node(h->array_list[i], 0);
	//  il adaug in lista de memorie alocata
	add_node_by_address(a_list, r, a_size);
	// verific daca trebuie fragmentat blocul
	// daca nu m-ai am elemente in lista i o sterg
	int new_size = h->array_list[i]->b_size - a_size;
	if (!h->array_list[i]->head)
		realloc_array_list(h, i, 0, a_size);
	int ok = check_list_size(h, new_size);
	int j = 0;
	// adaug noua pozitie in vectorul de liste
	if (new_size) {
		h->n_fragmentation++;
		while (j < i) {
			if (new_size <= h->array_list[j]->b_size)
				break;
			j++;
		}
		// daca noua dimensiune nu exista realoc vectorul
		if (!ok)
			realloc_array_list(h, j, 1, new_size);
		node_t *aux = malloc(sizeof(node_t));
		aux->address = r->address + a_size;
		add_node_by_address(h->array_list[j], aux, new_size);
		free(aux);
	}
	free(r);
}

// initializez heapul , prin creearea vectorului de liste in ordinea
// crescatoare a adreselor in functie de informatiile primite si initializez
// datele necesare la afisarea memoriei la dump
void init_heap(sfl_t *h, list_t *alloc)
{
	scanf("%lx %d %d %d", &h->start_address, &h->nr_list, &h->bytes, &h->type);
	h->array_list = malloc(h->nr_list * sizeof(list_t *));
	size_t addr = h->start_address;
	int size = h->bytes;
	size = size >> 3;
	for (int i = 0; i < h->nr_list; i++) {
		h->array_list[i] = create_list(h->bytes / size);
		for (int j = 0; j < size; j++) {
			node_t *node = malloc(sizeof(node_t));
			add_nth_node(h->array_list[i], j, addr, h->bytes / size);
			addr += h->bytes / size;
			free(node);
		}
		size = size >> 1;
	}
	h->bytes_allocated = 0;
	h->n_fragmentation = 0;
	h->n_free = 0;
	h->n_malloc = 0;
	h->total_mem = h->bytes * (h->nr_list);
}

// functie care adauga un nod in vectorul de liste la comanda free
//  aceasta verifica daca dimensiunea blocului exista in liste si
// o adauga in caz ca nu exista
void add_node_to_heap(sfl_t *h, node_t *r)
{
	int ok = check_list_size(h, r->bytes);
	int j = 0;
	while (j < h->nr_list) {
		if (r->bytes <= h->array_list[j]->b_size)
			break;
		j++;
	}
	if (!ok)
		realloc_array_list(h, j, 1, r->bytes);
	add_node_by_address(h->array_list[j], r, r->bytes);
}

// functia free folosita cand h-type = 0 , adica fiecare bloc se duce in heap
// in lista corespunzatoare dimensiunii blocului
void free_basic(sfl_t *h, list_t *alloc)
{
	size_t addr;
	scanf("%lx", &addr);
	node_t *curr = alloc->head;
	unsigned int crt = 0;
	while (curr && addr != curr->address) {
		curr = curr->next;
		crt++;
	}
	if (!curr) {
		printf("Invalid free\n");
		return;
	}
	h->n_free++;
	h->bytes_allocated -= curr->bytes;
	// elimin  nodul din lista alocata
	node_t *r = remove_nth_node(alloc, crt);
	if (r->data)
		free(r->data);
	// il adaug in heap
	add_node_to_heap(h, r);
	free(r);
}

// functia aceasta o folosesc la free bonus atunci cand h->type = 1
// aceasta funcite verifica daca doua blocuri au facut parte din aceeasi lista
// initiala si dupa si din acelasi bloc din respectiva lista
// daca cele doua blocuri verifica ambele conditii returneaza 1 , altfel 0
int check_initial_block_from_heap(sfl_t *h, node_t *n1, node_t *n2)
{
	// verific dimensiunea listei initiale a celor doua blocuri
	int size1 = (n1->address - h->start_address) / h->bytes;
	int size2 = (n2->address - h->start_address) / h->bytes;
	if (size1 != size2)
		return 0;
	// daca cele doua blocuri au facut parte din aceeasi lista
	// calculez scazand din adresa de start a fiecarei liste adresa
	// fiecarui bloc si impart la dimensiunea initiala a listei
	size_t start_addr = h->start_address + size1 * h->bytes;
	int size = 8 << size1;
	int initial_block1 = (n1->address - start_addr) / size;
	int initial_block2 = (n2->address - start_addr) / size;
	if (initial_block1 != initial_block2)
		return 0;
	return 1;
}

// la free bonus lipesc doua blocuri la dreapta pentru a reface blocul intial
// dupa ce am verificat inainte daca cele doua blocuri faceau parte din acelasi
// bloc la inceputul heapului
// la comanda free parcurg tot vectorul de liste si verific daca blocul
// respectiv se poate lipi cu alt bloc din sfl la dreapta
void remake_initial_block_to_right(sfl_t *h, size_t addr, node_t *rm)
{
	for (int i = 0; i < h->nr_list; i++) {
		node_t *curr = h->array_list[i]->head;
		int j = 0;
		while (curr) {
			int ok = check_initial_block_from_heap(h, rm, curr);
			if (rm->address + rm->bytes == curr->address && ok == 1) {
				// daca am gasit un bloc il elimin , verific daca mai am
				// elemente in vectorul de liste si dupa modific dimensiunea
				node_t *r2 = remove_nth_node(h->array_list[i], j);
				if (!h->array_list[i]->head)
					realloc_array_list(h, i, 0, r2->bytes);
				rm->bytes += r2->bytes;
				free(r2);
				return;
			}
			j++;
			curr = curr->next;
		}
	}
}

// la free bonus lipesc doua blocuri la stanga pentru a reface blocul intial
// dupa ce am verificat inainte daca cele doua blocuri faceau parte din acelasi
// bloc la inceputul heapului
// la comanda free parcurg tot vectorul de liste si verific daca blocul
// respectiv se poate lipi cu alt bloc din sfl la stanga
void remake_initial_block_to_left(sfl_t *h, size_t addr, node_t *rm)
{
	for (int i = 0; i < h->nr_list; i++) {
		node_t *curr = h->array_list[i]->head;
		int j = 0;
		while (curr) {
			int ok = check_initial_block_from_heap(h, rm, curr);
			if (rm->address == curr->address + curr->bytes && ok == 1) {
				// daca am gasit un bloc il elimin , verific daca mai am
				// elemente in vectorul de liste si dupa modific dimensiunea
				node_t *r2 = remove_nth_node(h->array_list[i], j);
				if (!h->array_list[i]->head)
					realloc_array_list(h, i, 0, r2->bytes);
				rm->bytes += r2->bytes;
				rm->address = r2->address;
				free(r2);
				return;
			}
			j++;
			curr = curr->next;
		}
	}
}

void free_bonus(sfl_t *h, list_t *alloc)
{
	size_t addr;
	scanf("%lx", &addr);
	node_t *curr = alloc->head;
	unsigned int crt = 0;
	while (curr && addr != curr->address) {
		curr = curr->next;
		crt++;
	}
	if (!curr) {
		printf("Invalid free\n");
		return;
	}
	h->n_free++;
	h->bytes_allocated -= curr->bytes;
	// elimin  nodul din lista alocata
	node_t *r = remove_nth_node(alloc, crt);
	// la fiecare comanda free verific daca fiecare bloc se poate unii atat la
	// stanga cat si la dreapta este suficient sa fac asta o singura data
	// pentru ca orice bloc se afla in heap in acel moment a fost clar lipit
	// inaite daca era cazul la un free anterior
	remake_initial_block_to_right(h, addr, r);
	remake_initial_block_to_left(h, addr, r);
	add_node_to_heap(h, r);
	free(r);
}

// la comanda free mut blocurile din lista de alocate in vectorul de liste
void free_list_heap(sfl_t *h, list_t *alloc)
{
	if (h->type == 0)
		free_basic(h, alloc);
	else
		free_bonus(h, alloc);
}

// dau free la lista , inclusiv la elemntele ei
void ll_free(list_t **pp_list)
{
	node_t *p = (*pp_list)->head;
	while (p) {
		node_t *aux = p;
		p = p->next;
		if (aux->data)
			free(aux->data);
		free(aux);
	}
	free(*pp_list);
}

// printez vectorul de liste pentru comanda dump
void print_lists(sfl_t *h)
{
	for (int i = 0; i < h->nr_list; i++) {
		int bytes = h->array_list[i]->b_size;
		int size = h->array_list[i]->size;
		printf("Blocks with %d bytes - %d free block(s) : ", bytes, size);
		node_t *current = h->array_list[i]->head;
		while (current) {
			if (current->next)
				printf("0x%lx ", current->address);
			else
				printf("0x%lx", current->address);
			current = current->next;
		}
		printf("\n");
	}
}

// afisez detaliile heapului in momentul cand se apeleaza comanda "DUMP_MEMORY"
void dump_memory(sfl_t *h, list_t *alloc)
{
	unsigned long free_blocks = 0;
	printf("+++++DUMP+++++\n");
	printf("Total memory: %d bytes\n", h->total_mem);
	printf("Total allocated memory: %d bytes\n", h->bytes_allocated);
	printf("Total free memory: %d bytes\n", h->total_mem - h->bytes_allocated);
	for (int i = 0; i < h->nr_list; i++)
		free_blocks += h->array_list[i]->size;
	printf("Free blocks: %lu\n", free_blocks);
	printf("Number of allocated blocks: %d\n", alloc->size);
	printf("Number of malloc calls: %d\n", h->n_malloc);
	printf("Number of fragmentations: %d\n", h->n_fragmentation);
	printf("Number of free calls: %d\n", h->n_free);
	print_lists(h);
	printf("Allocated blocks :");
	node_t *current = alloc->head;
	while (current) {
		printf(" (0x%lx - %d)", current->address, current->bytes);
		current = current->next;
	}
	printf("\n");
	printf("-----DUMP-----\n");
}

// distrug intreagul heap
// si eliberez toata memoria pentru inchiderea programului
void destroy_heap(sfl_t *h, list_t *alloc, int *loop)
{
	(*loop) = 0;
	for (int i = 0; i < h->nr_list; i++)
		ll_free(&h->array_list[i]);
	free(h->array_list);
	ll_free(&alloc);
}

// gasesc cuvintele pe care trebuie sa le scriu la comanda WRITE in blocuri
int find_words(words_t *w)
{
	char delim[] = " '\n'\"";
	int nr = 0;
	for (char *p = strtok((*w).sentence, delim); p; p = strtok(NULL, delim)) {
		strcpy((*w).words[nr], p);
		nr++;
	}
	return nr;
}

// transform numarul citit in int, dupa ce am citit
// fraza care trebuie scrisa la write
int cifreint(char c[])
{
	int n = strlen(c);
	char p[LG_MAX];
	int nr = 0;
	strcpy(p, c);
	for (int i = 0; i < n; i++)
		nr = nr * 10 + (c[i] - '0');
	return nr;
}

// verific daca am o adresa consecutiva care sa aiba dimensiunea nr_bytes_left
// si care sa inceapa de la adresa cititia daca nu , nu se poate efectuea
// comanda de write
int check_write(list_t *alloc, size_t addr, int nr_bytes_left)
{
	node_t *curr = alloc->head;
	while (curr && addr != curr->address)
		curr = curr->next;
	while (nr_bytes_left) {
		int nr_bytes_to_write;
		if (curr && curr->address == addr) {
			if (nr_bytes_left > curr->bytes)
				nr_bytes_to_write = curr->bytes;
			else
				nr_bytes_to_write = nr_bytes_left;
			nr_bytes_left -= nr_bytes_to_write;
		} else {
			return 0;
		}
		addr += curr->bytes;
		curr = curr->next;
	}
	return 1;
}

// in caz ca functiile write, free sau read esueaza apelez aceasta functie
// si inchid programul
void seg_fault(sfl_t *h, list_t *alloc, int *loop)
{
	printf("Segmentation fault (core dumped)\n");
	dump_memory(h, alloc);
	destroy_heap(h, alloc, loop);
}

// la comanda write scriu la adresa citita o anumita fraza
void write(sfl_t *h, list_t *alloc, int *loop)
{
	size_t addr;
	scanf("%lx", &addr);
	words_t w;
	fgets(w.sentence, LG_MAX, stdin);
	int nr = find_words(&w);
	strcpy(w.sentence, "\0");
	// refac propozitia initiala la care am eliminat ""
	for (int i = 0; i < nr - 1; i++) {
		strcat(w.sentence, w.words[i]);
		if (i != nr - 2)
			strcat(w.sentence, " ");
	}
	int nr_bytes_left = cifreint(w.words[nr - 1]);
	if (nr_bytes_left > strlen(w.sentence))
		nr_bytes_left = strlen(w.sentence);
	int check = check_write(alloc, addr, nr_bytes_left);
	if (check == 0) {
		seg_fault(h, alloc, loop);
		return;
	}
	node_t *curr = alloc->head;
	while (curr && addr != curr->address)
		curr = curr->next;
	while (nr_bytes_left) {
		int nr_bytes_to_write;
		if (curr && curr->address == addr) {
			// daca am gasit adresa corespunzatoare verific cati bytes mai
			// trebuie scris
			char *copy;
			if (nr_bytes_left > curr->bytes)
				nr_bytes_to_write = curr->bytes;
			else
				nr_bytes_to_write = nr_bytes_left;
			// verific daca trebuie sa suprascriu un numar de bytes
			if (!curr->data) {
				curr->data = malloc((curr->bytes + 1) * sizeof(char));
				memcpy(curr->data, w.sentence, nr_bytes_to_write);
				memcpy(curr->data + nr_bytes_to_write, "\0", 1);
				char *aux = malloc(LG_MAX * sizeof(char));
				strcpy(aux, w.sentence);
				strcpy(w.sentence, aux + nr_bytes_to_write);
				free(aux);
			} else {
				// copiez vechea informatie pentru a o concatena la final
				// ceea ce nu trebuia suprascris
				copy = malloc((strlen((char *)curr->data) + 1) * sizeof(char));
				memcpy(copy, (char *)curr->data, strlen((char *)curr->data));
				memcpy(copy + strlen((char *)curr->data), "\0", 1);
				memcpy(curr->data, w.sentence, nr_bytes_to_write);
				memcpy(curr->data + nr_bytes_to_write, "\0", 1);
				char *aux = malloc(LG_MAX * sizeof(char));
				strcpy(aux, w.sentence);
				strcpy(w.sentence, aux + nr_bytes_to_write);
				free(aux);
				if (strlen(copy) > strlen((char *)curr->data)) {
					int nr_to_copy = strlen(copy) - strlen((char *)curr->data);
					int by_to_move = strlen((char *)curr->data);
					memcpy(curr->data + by_to_move, copy + by_to_move, nr);
				}
				free(copy);
			}
			// scad din numarul de bytes pe care trebuie sa i scriu
			// pe cei pe car ei am scris
			nr_bytes_left -= nr_bytes_to_write;
		} else {
			seg_fault(h, alloc, loop);
			return;
		}
		addr += curr->bytes;
		curr = curr->next;
	}
}

// verific daca am o adresa consecutiva care sa aiba dimensiunea nr_bytes_left
// si care sa inceapa de la adresa cititia daca nu , nu se poate nu se efectua
// comanda de read si se afiseaza segmentation fault
int check_read(list_t *alloc, size_t addr, int nr_bytes_left)
{
	node_t *curr = alloc->head;
	while (nr_bytes_left) {
		int nr_bytes_to_read;
		while (curr && addr - curr->address >= curr->bytes)
			curr = curr->next;
		if (curr && curr->data) {
			if (nr_bytes_left > curr->bytes - addr + curr->address)
				nr_bytes_to_read = curr->bytes - addr + curr->address;
			else
				nr_bytes_to_read = nr_bytes_left;
			nr_bytes_left -= nr_bytes_to_read;
			if (addr != curr->address)
				addr = 0;
		} else {
			return 0;
		}
		addr += curr->bytes;
		curr = curr->next;
	}
	return 1;
}

// functia care citeste un numar de bytes de la o anumita adresa
void read(sfl_t *h, list_t *alloc, int *loop)
{
	int nr_bytes_left;
	size_t addr;
	scanf("%lx%d", &addr, &nr_bytes_left);
	node_t *curr = alloc->head;
	int check = check_read(alloc, addr, nr_bytes_left);
	if (check == 0) {
		seg_fault(h, alloc, loop);
		return;
	}
	// la fiecare cititre scad numarul denbytes cititi
	while (nr_bytes_left) {
		int nr_bytes_to_read;
		while (curr && addr - curr->address >= curr->bytes)
			curr = curr->next;
		if (curr) {
			if (nr_bytes_left > curr->bytes - addr + curr->address)
				nr_bytes_to_read = curr->bytes - addr + curr->address;
			else
				nr_bytes_to_read = nr_bytes_left;
			char *copy = malloc((nr_bytes_to_read + 1) * sizeof(char));
			int bytes_to_move = addr - curr->address;
			memcpy(copy, (char *)curr->data + bytes_to_move, nr_bytes_to_read);
			memcpy(copy + nr_bytes_to_read, "\0", 1);
			printf("%s", copy);
			free(copy);
			nr_bytes_left -= nr_bytes_to_read;
			// daca inital adresa nu incepea de la inceputul blocului
			// o modific pentru a putea citi incontinuare de la
			// inceputul urmatorului bloc
			if (addr != curr->address)
				addr = curr->address;
		} else {
			seg_fault(h, alloc, loop);
			return;
		}
		addr += curr->bytes;
		curr = curr->next;
	}
	printf("\n");
}

int main(void)
{
	char command[LG_MAX];
	sfl_t h;
	list_t *alloc_list = create_list(0);
	int loop = 1;
	while (loop) {
		scanf("%s", command);
		if (!strcmp(command, "INIT_HEAP"))
			init_heap(&h, alloc_list);
		if (!strcmp(command, "MALLOC"))
			my_malloc(&h, alloc_list);
		if (!strcmp(command, "DUMP_MEMORY"))
			dump_memory(&h, alloc_list);
		if (!strcmp(command, "FREE"))
			free_list_heap(&h, alloc_list);
		if (!strcmp(command, "WRITE"))
			write(&h, alloc_list, &loop);
		if (!strcmp(command, "READ"))
			read(&h, alloc_list, &loop);
		if (!strcmp(command, "DESTROY_HEAP"))
			destroy_heap(&h, alloc_list, &loop);
	}
	return 0;
}

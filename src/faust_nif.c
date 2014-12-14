#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <time.h>
#include <limits.h>
#include <string.h>
#include <math.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include "erl_nif.h"

struct transition_t {
	struct transition_t	*next;
	char*			value;
	int			hits;
	double			likelihood;
};

struct node_t { 
	int			t_count;
	char*			key;
	struct node_t		*next;
	struct transition_	*tr;
};

typedef struct {
	ErlNifThreadOpts	*opts;
	ErlNifTid		_thread;
	ErlNifPid		_pid;
	int		n;
	int		order;
	int		table_size;
	char*		path;
	char		iter;
	char**		iter_hist;
	struct node_t	**table; 
} _state_t;

void nif_thread_send(char* msg, _state_t *state) {
	ErlNifEnv *env = enif_alloc_env();
	enif_send(NULL, &state->_pid, env, enif_make_string(env, msg, ERL_NIF_LATIN1));
	sleep(1);
	enif_clear_env(env);
}

// Box Jenkins >_>
int hash(char* key, _state_t *state) {
    	size_t hash, i;
    	for (hash = i = 0; i < strlen(key); ++i) {
    		hash += key[i];
    		hash += (hash << 10);
	  	hash ^= (hash >> 6);
	   	hash += (hash << 3); 
	  	hash ^= (hash >> 11);
	   	hash += (hash << 15);
	}
	return hash % state->table_size;
}
 
struct transition_t *init_transition(char* value) {
	struct transition_t *new_transition;
	if ((new_transition = malloc(sizeof(struct transition_t *))) == NULL) return NULL;	
	if ((new_transition->value = strdup(value)) == NULL) return NULL;
	new_transition->next = NULL;
	new_transition->hits = 1;
	return new_transition;
}

struct node_t *init_node(char* key, char* value) {
	struct node_t *new_node;
	if ((new_node = malloc(sizeof(struct node_t *))) == NULL) return NULL;	
 	if ((new_node->key = strdup(key)) == NULL) return NULL;
	new_node->next = NULL;
	new_node->t_count = 1;
	new_node->tr = init_transition(value);
	return new_node;
}

void insert(char *key, char *value, _state_t* state) {
	int i = hash(key, state);
	struct node_t *new_node = NULL;
	struct node_t *last = NULL;
	struct node_t *next = state->table[i];
	struct transition_t *transition = NULL;
	char* buf;
	while (next != NULL && next->key != NULL && strcmp(key, next->key) > 0) {
		last = next;
		next = next->next;
	}
	if (next != NULL && next->key != NULL && strcmp(key, next->key) == 0) {	
		transition = next->tr;
		while (transition->next != NULL && strcmp(value, transition->value) > 0) {
			transition = transition->next;
		}
		if (strcmp(transition->value, value) == 0) transition->hits++;	
 		else if (transition->next == NULL) { 
			transition->next = init_transition(value);
			next->t_count++;
		}
	}
	else {		
		new_node = init_node(key, value);
		if (next == state->table[i]) {		
			new_node->next = next;
			state->table[i] = new_node;
		}
		else if (next == NULL) last->next = new_node; 
		else  {		
			new_node->next = next;
			last->next = new_node;
		}
	}
}
 
struct node_t *fetch(char *key, _state_t* state) {
	int i = 0;
	struct node_t *node;
	i = hash(key, state);
	node = state->table[i];							
	while (node != NULL && node->key != NULL && strcmp(key, node->key) > 0) node = node->next;
	if (node == NULL || node->key == NULL || strcmp(key, node->key) != 0) return NULL;
	else return node;
}

char* merge(char* base, char* str) {
	char* res = (char*) malloc(1 + strlen(base) + strlen(str));
	if (res == NULL) return NULL;
	memcpy(res, base, strlen(base));
	memcpy((res + strlen(base)), str, (1+strlen(str)));
	return res;
}

struct transition_t *choose_transition(struct node_t *node, _state_t *state) {
	int i, buf, flag;
	struct transition_t *transition = node->tr;
	struct transition_t *selection;
	double x;
	srand48(time(NULL));
	x = (double) drand48();
	for (i = 0; i < node->t_count; i++) {
		if(buf = (transition->likelihood - x) < flag) {
			flag = buf;
			selection = transition;	
		}
		transition = transition->next;
	}
	return selection;
}

char* generate_text(_state_t *state) {
	int i, q;
	char* acc;
	struct node_t *node = fetch(state->iter, state);
	struct transition_t *transition = choose_transition(node, state);
	acc = merge(state->iter, " ");
	for (i = 0; i < state->n; i++) {
		for (q = 0; q < (state->order-1); ++q) state->iter_hist[q] = state->iter_hist[1+q];
		state->iter_hist[state->order-1] = transition->value;
		state->iter = merge("", state->iter_hist[0]);
		for (q = 1; q < state->order; ++q) state->iter = merge(state->iter, state->iter_hist[q]);
		node = fetch(state->iter, state);	
		if (node == NULL) perror("transition value not found in table");
		transition = choose_transition(node, state);
		nif_thread_send(transition->value, state);		
		acc = merge(acc, transition->value);
		acc = merge(acc, " ");
	}
	return acc;
}

void calculate_transition_distribution(struct node_t *node, _state_t *state) {
	int i;
	struct transition_t *transition = node->tr;
	for (i = 0; i < node->t_count; i++) {
		transition->likelihood = (double) transition->hits/node->t_count;
		transition = transition->next;	
	}
}

void build_chain(char** arr, int arr_size, _state_t *state) {
	int i, q, j;
	char* buf;
	for (q = 0; q < state->order; ++q) state->iter_hist[q] = strdup(arr[q]);
	for (i = 0; (i + state->order) < arr_size; ++i) {
		for (j = 1; j < state->order; ++j) {
			if (j == 1) buf = merge(arr[i], arr[i + j]);
			else buf = merge(buf, arr[i + j]);
		}
		if (i == 0) state->iter = buf;
		insert(buf, arr[(i + state->order)], state);		
	}
	for (i = 0; (i + state->order) < arr_size; ++i) {
		for (j = 1; j < state->order; ++j) {
			if (j == 1) buf = merge(arr[i], arr[i + j]);
			else buf = merge(buf, arr[i + j]);
		}
		calculate_transition_distribution(fetch(buf, state), state);		
	}
	generate_text(state);
	free(arr);
}

void init_table(int size, _state_t *state) {
	int i;
	if (size < 1) perror("memory");
	if ((state->table = malloc(sizeof(struct node_t *) * size)) == NULL) perror("memory");
	for (i = 0; i < size; i++) state->table[i] = NULL;
	state->table_size = size;
}
 
void process(char* corpus, _state_t *state) {
	int q = 0;
	char** arr = NULL;
	char* fragment = strtok(corpus, " ");
	while (fragment) {
		arr = realloc(arr, sizeof(char*) * ++q);
		arr[q-1] = fragment;
		fragment = strtok(NULL, " ");
	}
	arr = realloc(arr, sizeof(char*) * (1+q));
	arr[q] = '\0';
	state->iter_hist = malloc(sizeof(char*) * state->order);
	init_table(128+q, state);
	build_chain(arr, q, state);
}

char* remove_newlines(char* corpus, int s) {
	int i = 0, q = 0;
	char* buf = malloc(sizeof(char*));
	for (i = 0; i < s; ++i) {
		buf = realloc(buf, sizeof(char*) * ++q);
		if (corpus[i] == '\n') buf[q-1] = ' ';
		else buf[q-1] = corpus[i];
	}
	return buf;
}

static void *worker(void *obj) {
	off_t fs;
	char *corpus;
	struct stat stbuf;
	_state_t *state = (_state_t *) obj;
	int fd = open((char*) state->path, O_RDONLY);
	if (fd == -1) nif_thread_send("file error", state);
	if ((fstat(fd, &stbuf) != 0) || (!S_ISREG(stbuf.st_mode))) nif_thread_send("file error", state);
	fs = stbuf.st_size;
	corpus = malloc((1+fs) * (sizeof(char)));
	read(fd, corpus, fs);
	corpus[fs] = '\0';
	close(fd);
	process(remove_newlines(corpus, fs), state);
	return NULL;
}

static ERL_NIF_TERM _start_(ErlNifEnv *env, int argc, const ERL_NIF_TERM argv[]) {
	ErlNifBinary path_bin;
	_state_t *state = (_state_t *) enif_alloc(sizeof(_state_t));
	if (state == NULL) return enif_make_badarg(env);
 	if (enif_inspect_iolist_as_binary(env, argv[0], &path_bin)) {
		state->path = path_bin.data;
		state->path[path_bin.size] = '\0';
	}
	else perror("input path");
	enif_get_int(env, argv[1], &state->order);
	enif_get_int(env, argv[2], &state->n);
	state->opts = enif_thread_opts_create("thr_opts");
	enif_self(env, &state->_pid);
	if (enif_thread_create("", &state->_thread, worker, state, state->opts) != 0) return enif_make_badarg(env);
	return enif_make_atom(env, "ok");
}

static ErlNifFunc nif_funcs[] = {{"call_andrey", 3, _start_}};
ERL_NIF_INIT(Elixir.Faust, nif_funcs, NULL, NULL, NULL, NULL);

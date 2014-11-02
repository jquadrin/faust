#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>
#include "erl_nif.h"

#define HASHSIZE 10000

struct transition {
	struct transition *next;
	char* value;
	int hits;
};

struct node { 
	struct node *next; 
	char* key; 
	struct transition *_t;
	int t_count;
};

typedef struct {
	ErlNifThreadOpts  	*opts;
	ErlNifTid   	  	_thread;
	ErlNifPid         	_pid;
	char*     	  	path;
	struct node 		*table[HASHSIZE]; 
} process_state_t;

typedef enum { false, true } bool;

unsigned hash(char *s) {
	unsigned hashval;
	for (hashval = 0; *s != '\0'; s++)
		hashval = *s + 31 * hashval;
	return hashval % HASHSIZE;
}

struct node *find_node(char *key, process_state_t *process_state) {
	struct node *_node;
	for (_node = process_state->table[hash(key)]; _node != NULL; _node = _node->next) {
		if (strcmp(key, _node->key) == 0){
			return _node;
		}
	}
	return NULL; 
}

struct transition *find_transition(char *value, struct node *_node, process_state_t *process_state) {
	ErlNifEnv *env = enif_alloc_env();
	struct transition *_transition;
	char *buf = _node->_t->value;
	if (buf == NULL)
	enif_send(NULL, &(process_state->_pid), env, enif_make_string(env, "val null!", ERL_NIF_LATIN1));
	else
	enif_send(NULL, &(process_state->_pid), env, enif_make_string(env, buf, ERL_NIF_LATIN1));

	enif_send(NULL, &(process_state->_pid), env, enif_make_string(env, value, ERL_NIF_LATIN1));
	sleep(1);
	for (_transition = _node->_t; _transition != NULL; _transition = _transition->next) {
		if (strcmp(value, _transition->value) == 0) {
	enif_send(NULL, &(process_state->_pid), env, enif_make_string(env, "FOUND", ERL_NIF_LATIN1));
	enif_clear_env(env);
			return _transition;
		}
	}
	enif_send(NULL, &(process_state->_pid), env, enif_make_string(env, "NULL", ERL_NIF_LATIN1));
	enif_clear_env(env);
	return NULL;
}

struct node *insert(char* key, char* value, process_state_t *process_state) {
	ErlNifEnv *env = enif_alloc_env();
	struct node *_node;
	struct transition *_transition;
	unsigned hashval;
	_node = find_node(key, process_state);
	if (_node == NULL) { 
		_node = (struct node *) malloc(sizeof(*_node));
		if (_node == NULL) return NULL;
		_node->key = strdup(key);
		
		//enif_send(NULL, &(process_state->_pid), env, enif_make_string(env, _node->key, ERL_NIF_LATIN1));
		if (_node->key == NULL) return NULL;
		hashval = hash(key);
		_node->t_count = 1;
		_node->next = process_state->table[hashval];
		process_state->table[hashval] = _node;
		//sleep(1);	
		//enif_send(NULL, &(process_state->_pid), env, enif_make_string(env, process_state->table[hashval]->key, ERL_NIF_LATIN1));
		_transition = (struct transition *) malloc(sizeof(*_transition));
		if (_transition == NULL) return NULL;
		_transition->value = strdup(value);
		if (_transition->value == NULL) return NULL;
		_transition->hits = 1;
		_node->_t = _transition;
		++_node->t_count;
      	} 
      	else { 	
		enif_send(NULL, &(process_state->_pid), env, enif_make_string(env, "----------------------------------------COLLISION", ERL_NIF_LATIN1));
		sleep(1);
		_transition = find_transition(value, _node, process_state);
		enif_send(NULL, &(process_state->_pid), env, enif_make_string(env, "transition search worked", ERL_NIF_LATIN1));
		if (_transition == NULL) {
			_transition = (struct transition *) malloc(sizeof(*_transition));
			if (_transition == NULL) return NULL;
			_transition->value = strdup(value);
			if (_transition->value == NULL) return NULL;
			_transition->hits = 0;
			_transition->next = _node->_t;
			_node->_t = _transition;
			++_node->t_count;	
		}
		else 
			++_transition->hits;
	}
	enif_clear_env(env);
	return _node;
}

char* merge(char* base, char* str) {
	char* res = (char*) malloc(1 + strlen(base) + strlen(str));
	if (res == NULL)
		return NULL;
	memcpy(res, base, strlen(base));
	memcpy((res + strlen(base)), str, (1 + strlen(str)));
	return res;
}

void build_chain(char** arr, int arr_size, int order, process_state_t *process_state) {
	int i, j;
	char* buf;
	ErlNifEnv *env = enif_alloc_env();
	/* Reorganize methods
	 * Debug node/transition insertion and retrieval (find_transition for())
	 * Dynamically resize hashtable
	 * Assign transition probabilities
	 * Build generator function
	 * */
	insert("joe", "first", process_state);
	insert("joe", "second", process_state);
	for (i = 0; (i + order) < arr_size; ++i) {
		for (j = 1; j < order; ++j) {
			if (j == 1) buf = merge(arr[i], arr[i + j]);
			else buf = merge(buf, arr[i + j]);
		}
		enif_send(NULL, &(process_state->_pid), env, enif_make_string(env, buf, ERL_NIF_LATIN1));
		sleep(1);
		insert(buf, arr[(i + order)], process_state);		
		enif_send(NULL, &(process_state->_pid), env, enif_make_string(env, process_state->table[hash(buf)]->_t->value, ERL_NIF_LATIN1));
		sleep(1);
	}
	free(arr);
	enif_send(NULL, &(process_state->_pid), env, enif_make_string(env, ":closed", ERL_NIF_LATIN1));
	enif_clear_env(env);
}

void process(char* corpus, int order, process_state_t *process_state) {
	int q = 0;
	char** arr = NULL;
	char* fragment = strtok(corpus, " ");
	while (fragment) {
		arr = realloc(arr, sizeof(char*) * ++q);
		arr[q - 1] = fragment;
		fragment = strtok(NULL, " ");
	}
	arr = realloc(arr, sizeof(char*) * (q + 1));
	arr[q] = '\0';
	build_chain(arr, q, order, process_state);
}

static void *worker(void *obj) {
	long fs;
	char* corpus;
	process_state_t *process_state = (process_state_t *) obj;
	FILE *fd = fopen((char*) &(process_state->path), "rb");
	fseek(fd, 0, SEEK_END);
	fs = ftell(fd);
	rewind(fd);
	corpus = malloc((fs + 1) * (sizeof(char)));
	fread(corpus, sizeof(char), fs, fd);
	fclose(fd);
	corpus[fs] = '\0';
	process(corpus, 4, process_state);
	return NULL;
}

static ERL_NIF_TERM _start_(ErlNifEnv *env, int argc, const ERL_NIF_TERM argv[]) {
	ErlNifBinary path_bin;
	char* path;
	process_state_t *process_state = (process_state_t *) enif_alloc(sizeof(process_state_t));
	if (process_state == NULL)
		return enif_make_badarg(env);
 	if (enif_inspect_iolist_as_binary(env, argv[0], &path_bin)) {
		path = malloc(path_bin.size + 1);
		memcpy(path, path_bin.data, path_bin.size);
		path[path_bin.size] = '\0';
		memcpy((char*) &(process_state->path), path, path_bin.size + 1);
	}
	else
	  	perror("input string");
	enif_self(env, &(process_state->_pid));
	process_state->opts = enif_thread_opts_create("thr_opts");
	if (enif_thread_create("", &(process_state->_thread), worker, process_state, process_state->opts) != 0)
		return enif_make_badarg(env);
	return enif_make_atom(env, "ok");
}

static ErlNifFunc nif_funcs[] = {{"_start_", 1, _start_}};
ERL_NIF_INIT(Elixir.Markov, nif_funcs, NULL, NULL, NULL, NULL);

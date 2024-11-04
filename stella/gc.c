#include <stdlib.h>
#include <stdio.h>

#include "runtime.h"
#include "gc.h"

/** Total allocated number of bytes (over the entire duration of the program). */
int total_allocated_bytes = 0;

/** Total allocated number of objects (over the entire duration of the program). */
int total_allocated_objects = 0;

int max_allocated_bytes = 0;
int max_allocated_objects = 0;

int total_reads = 0;
int total_writes = 0;

#define MAX_GC_ROOTS 1024
#define MAX_ALLOC_SIZE 4096
#define G1_ALLOC_SIZE (MAX_ALLOC_SIZE * 4)
#define MAX_CHANGED_NODES 4096

int gc_roots_max_size = 0;
int gc_roots_top = 0;
void **gc_roots[MAX_GC_ROOTS];

int changed_nodes_pointer = 0;
void *changed_nodes[MAX_CHANGED_NODES];

typedef struct {
  void* forwarded;
  stella_object object;
} gc_wrapper;

gen G0;
gen G1;

void* gc_alloc(const size_t size_in_bytes) {
  if (G0.from == NULL) {
    G0.from = malloc(sizeof(heap));
    G1.from = malloc(sizeof(heap));
    G1.to = malloc(sizeof(heap));

    G0.from->gen = 0;
    G0.from->size = MAX_ALLOC_SIZE;
    G0.from->start = malloc(MAX_ALLOC_SIZE);
    G0.from->next = G0.from->start;

    G1.from->gen = 1;
    G1.from->size = G1_ALLOC_SIZE;
    G1.from->start = malloc(G1_ALLOC_SIZE);
    G1.from->next = G1.from->start;

    G0.to = G1.from;

    G1.to->gen = 1;
    G1.to->size = G1_ALLOC_SIZE;
    G1.to->start = malloc(G1_ALLOC_SIZE);
    G1.to->next = G1.to->start;
  }

  const size_t gc_wrapper_size = size_in_bytes + sizeof(void*);

  void *result = try_alloc(&G0, gc_wrapper_size);
  if (result == NULL) {
    collect(&G0);

    printf("After garbage collecting:\n");
    print_gc_state();
    print_gc_alloc_stats();

    result = try_alloc(&G0, gc_wrapper_size);
  }

  if (result != NULL) {
    total_allocated_bytes += size_in_bytes;
    total_allocated_objects += 1;
    max_allocated_bytes = total_allocated_bytes;
    max_allocated_objects = total_allocated_objects;
  } else {
    exit(137);
  }

  return result;
}

void print_gc_roots() {
  printf("Roots:\n");
  for (int i = 0; i < gc_roots_top; i++) {
    printf("\tIndex: %-5d | Address: %-15p | %-15p\n",i, gc_roots[i], *gc_roots[i]);
  }
}

void print_gc_alloc_stats() {
  printf("Garbage collector (GC) statistics:\n");
  printf("Total memory allocation:  %d bytes (%d objects)\n", total_allocated_bytes, total_allocated_objects);
  printf("Maximum residency:        %d bytes (%d objects)\n", max_allocated_bytes, max_allocated_objects);
  printf("Total memory use:         %d reads and %d writes\n", total_reads, total_writes);
}

void print_gc_state() {
  print_state(&G0);
  print_state(&G1);
  print_gc_roots();
}

size_t get_gc_wrapper_size(const gc_wrapper *obj) {
  const int fields_count = STELLA_OBJECT_HEADER_FIELD_COUNT(obj->object.object_header);
  return sizeof(gc_wrapper) + fields_count * sizeof(void*);
}

void print_heap(const heap* heap) {
  printf("Objects:\n");
  for (void *start = heap->start; start < heap->next; start += get_gc_wrapper_size(start)) {
    const gc_wrapper *gc_ptr = start;
    printf("\tAddress: %-15p | Fields: ", gc_ptr);

    const int field_count = STELLA_OBJECT_HEADER_FIELD_COUNT(gc_ptr->object.object_header);
    for (int i = 0; i < field_count; i++) {
      printf("%-15p", gc_ptr->object.object_fields[i]);
      if (i < field_count - 1) {
        printf(" ");
      }
    }

    printf("\n");
  }

  printf("From: %-15p | To: %-15p | Size: %d bytes\n", heap->start,heap->start + heap->size,heap->size);
}

void print_state(const gen* g) {
  printf("G%d state\n", g->from->gen);
  printf("Cycles count %d\n", g->cycles_count);
  print_heap(g->from);

  if (g->to->gen == g->from->gen) {
    printf("To\n");
    print_heap(g->to);
  }
}

void gc_read_barrier(void *object, int field_index) {
  total_reads += 1;
}

void gc_write_barrier(void *object, int field_index, void *contents) {
  total_writes += 1;

  changed_nodes[changed_nodes_pointer] = object;
  changed_nodes_pointer++;
}

void gc_push_root(void **ptr){
  gc_roots[gc_roots_top++] = ptr;
  if (gc_roots_top > gc_roots_max_size) { gc_roots_max_size = gc_roots_top; }
}

void gc_pop_root(void **ptr){
  gc_roots_top--;
}

int is_in_heap(const heap* heap, const void* ptr) {
  return ptr >= heap->start && ptr < heap->start + heap->size;
}

gc_wrapper* alloc_in_heap(heap* heap, const size_t requested_size) {
  if (heap->next + requested_size <= heap->start + heap->size) {
    gc_wrapper *result = heap->next;
    result->forwarded = NULL;
    result->object.object_header = 0;
    heap->next += requested_size;

    return result;
  }

  return NULL;
}

stella_object* get_stella_object(gc_wrapper* gc_ptr) {
  return &gc_ptr->object;
}

void* try_alloc(const gen* g, const size_t size_in_bytes) {
  void* allocated = alloc_in_heap(g->from, size_in_bytes);
  if (allocated == NULL) {
    return NULL;
  }

  return allocated + sizeof(void*);
}

int chase(const gen* g, gc_wrapper *p) {
  do {
    gc_wrapper *q = alloc_in_heap(g->to, get_gc_wrapper_size(p));
    if (q == NULL) {
      return 0;
    }

    void *r = NULL;
    q->forwarded = NULL;
    q->object.object_header = p->object.object_header;
    const int field_count = STELLA_OBJECT_HEADER_FIELD_COUNT(p->object.object_header);
    for (int i = 0; i < field_count; i++) {
      q->object.object_fields[i] = p->object.object_fields[i];

      if (is_in_heap(g->from, q->object.object_fields[i])) {
        gc_wrapper *forwarded = q->object.object_fields[i] - sizeof(void*);

        if (!is_in_heap(g->to, forwarded->forwarded)) {
          r = forwarded;
        }
      }
    }

    p->forwarded = q;
    p = r;
  } while (p != NULL);

  return 1;
}

void* forward(gen* g, void* p) {
  if (!is_in_heap(g->from, p)) {
    return p;
  }

  gc_wrapper* gc_wrapper = p - sizeof(void*);

  if (is_in_heap(g->to, gc_wrapper->forwarded)) {
    return get_stella_object(gc_wrapper->forwarded);
  }

  int chase_result = chase(g, gc_wrapper);
  if (!chase_result) {
    if (g->to->gen == g->from->gen) {
      exit(137);
    }

    collect(g->to->gen == 0 ? &G0 : &G1);

    chase_result = chase(g, gc_wrapper);
    if (!chase_result) {
      exit(137);
    }
  }
  return get_stella_object(gc_wrapper->forwarded);
}

void collect(gen* g) {
  g->cycles_count++;
  printf("Before garbage collect G%d number %d\n", g->from->gen, g->cycles_count);
  print_gc_state();
  g->scan = g->to->next;
  for (int i = 0; i < gc_roots_top; i++) {
    void **root_ptr = gc_roots[i];
    *root_ptr = forward(g, *root_ptr);
  }
  if (g->from->gen == 1) {
    for (void *ptr = G0.from->start; ptr < G0.from->next; ptr += get_gc_wrapper_size(ptr)) {
      gc_wrapper *obj = ptr;
      const int field_count = STELLA_OBJECT_HEADER_FIELD_COUNT(obj->object.object_header);
      for (int j = 0; j < field_count; j++) {
        obj->object.object_fields[j] = forward(g, obj->object.object_fields[j]);
      }
    }
  }

  for (int i = 0; i < changed_nodes_pointer; i++) {
    stella_object *obj = changed_nodes[i];
    const int field_count = STELLA_OBJECT_HEADER_FIELD_COUNT(obj->object_header);
    for (int j = 0; j < field_count; j++) {
      obj->object_fields[j] = forward(g, obj->object_fields[j]);
    }

    changed_nodes_pointer = 0;
  }

  while (g->scan < g->to->next) {
    gc_wrapper *obj = g->scan;
    const int field_count = STELLA_OBJECT_HEADER_FIELD_COUNT(obj->object.object_header);
    for (int i = 0; i < field_count; i++) {
      obj->object.object_fields[i] = forward(g, obj->object.object_fields[i]);
    }

    g->scan += get_gc_wrapper_size(obj);
  }

  if (g->from->gen == g->to->gen) {
    void *buff = g->from;
    g->from = g->to;
    g->to = buff;

    g->to->next = g->to->start;

    G0.to = g->from;
    G0.scan = g->from->start;
  } else {
    gen* current = g->from->gen == 0 ? &G0 : &G1;
    const gen* next = g->to->gen == 0 ? &G0 : &G1;
    current->from->next = g->from->start;
    current->to = next->from;
  }
}
#include <stdlib.h>
#include <stdbool.h>
#include <limits.h>

#include "min_heap.h"

/* - - - - - - - - - - - - - - - - - - - - */

typedef struct {
  int key;
  void *value;
} node;

struct min_heap {
  int size;
  int avail;
  node *nodes;
};

/* - - - - - - - - - - - - - - - - - - - - */

static inline int parent(int i);
static inline int left(int   i);
static inline int right(int  i);
static inline void exchange(node *nodes, int a, int b);
static void min_heapify(min_heap_t h, int i);
static void min_heap_resize(min_heap_t h);
static void min_heap_grow(min_heap_t h);
static void min_heap_shrink(min_heap_t h);

/* - - - - - - - - - - - - - - - - - - - - */

min_heap_t
min_heap_new(void)
{
  min_heap_t h = malloc(sizeof(*h));
  if (h) {
    h->size = 0;
    h->avail = 0;
    h->nodes = NULL;
  }
  return h;
}

int
min_heap_size(min_heap_t h)
{
  return h->size;
}

bool
min_heap_is_empty(min_heap_t h)
{
  return h->size == 0;
}

void
min_heap_insert(min_heap_t h, int k, void *v)
{
  if (h->size + 1 > h->avail) {
    min_heap_grow(h);
  }
  node *n = &h->nodes[h->size];
  n->key = INT_MAX;
  n->value = v;
  min_heap_decrease_key(h, h->size, k);
  h->size++;
}

void
min_heap_decrease_key(min_heap_t h, int i, int key)
{
  node *nodes = h->nodes;
  nodes[i].key = key;
  while (i > 0 && nodes[parent(i)].key > nodes[i].key) {
    exchange(nodes, i, parent(i));
    i = parent(i);
  }
}

void *
min_heap_minimum(min_heap_t h)
{
  return h->nodes[0].value;
}

void *
min_heap_remove_min(min_heap_t h)
{
  void *v = h->nodes[0].value;
  h->size--;
  h->nodes[0] = h->nodes[h->size];
  if (h->size < h->avail/2) {
    min_heap_shrink(h);
  }
  min_heapify(h, 0);
  return v;
}

void
min_heap_free(min_heap_t h)
{
  if (h->nodes) {
    free(h->nodes);
  }
  free(h);
}

static inline int parent(int i) { return i>>1;      }
static inline int left(int   i) { return i<<1;      }
static inline int right(int  i) { return left(i)+1; }

static inline void
exchange(node *nodes, int a, int b)
{
  node t = nodes[a];
  nodes[a] = nodes[b];
  nodes[b] = t;
}

static void
min_heapify(min_heap_t h, int i)
{
  node *nodes = h->nodes;
  int l = left(i);
  int r = right(i);
  int min = i;
  if (l <= h->size && nodes[l].key < nodes[min].key) { min = l; }
  if (r <= h->size && nodes[r].key < nodes[min].key) { min = r; }
  if (min != i) {
    exchange(nodes, i, min);
    min_heapify(h, min);
  }
}

static void
min_heap_resize(min_heap_t h)
{
  h->nodes = realloc(h->nodes, sizeof(node)*h->avail);
}

static void
min_heap_grow(min_heap_t h)
{
  h->avail = h->avail == 0 ? 1 : h->avail*2;
  min_heap_resize(h);
}

static void
min_heap_shrink(min_heap_t h)
{
  h->avail /= 2;
  min_heap_resize(h);
}

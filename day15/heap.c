#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>

#include "heap.h"

// Static functions
static void heapify(Heap *heap);
static int idxParent(int i);
static int idxLeftChild(int i);

// Creation
Heap *createHeap(int size, HEAP_TYPE type) {
  Heap *heap = (Heap*)calloc(sizeof(Heap), 1);
  heap->size = 0;
  heap->capacity = size;
  heap->data = (associated_data**)calloc(sizeof(associated_data*), size);
  heap->type = type;
  return heap;
}

Heap *createHeapData(associated_data *data, int size, HEAP_TYPE type) {
  Heap *heap = (Heap*)calloc(sizeof(Heap), 1);
  heap->size = size;
  heap->capacity = size;
  heap->data = (associated_data **)calloc(sizeof(associated_data*), size);
  for(int i = 0; i < size; i++) {
    associated_data *ad = (associated_data*)calloc(sizeof(associated_data), 1);
    ad->dist_ptr = data[i].dist_ptr;
    ad->idx = data[i].idx;
    ad->heap_idx = i;
    heap->data[i] = ad;
  }
  heap->type = type;
  heapify(heap);
  return heap;
}

// Basic
associated_data *peek(Heap *heap) {
  return heap->data[0];
}


associated_data *pop(Heap *heap) {
  associated_data *ret = heap->data[0];
  heap->data[0] = heap->data[heap->size - 1];
  heap->data[0]->heap_idx = 0;
  heap->data[heap->size - 1] = NULL;
  heap->size--;
  siftDown(heap->data, 0, heap->size - 1, heap->type);
  return ret;
}

// Inspection
int isEmpty(Heap *heap) {
  return heap->size == 0;
}

// Misc
void printHeap(Heap *heap) {
  for(int i = 0; i < heap->size; i++) {
    printf("%p(%d),%d", heap->data[i]->dist_ptr, *(heap->data[i]->dist_ptr), heap->data[i]->idx);
    if(i < heap->size-1)
      printf(" -> ");
  }
  putchar('\n');
}

// Internals
static void heapify(Heap *heap) {
  associated_data **data = heap->data;
  int size = heap->size;
  int start = idxParent(size - 1);
  while(start >= 0) {
    siftDown(data, start, size - 1, heap->type);
    start--;
  }
}

void siftDown(associated_data **data, int start, int end, HEAP_TYPE type) {
  int root = start;
  while(idxLeftChild(root) <= end) {
    int child = idxLeftChild(root);
    int swap = root;
    if((type == MAX_HEAP && *(data[swap]->dist_ptr) < *(data[child]->dist_ptr)) || 
        (type == MIN_HEAP && *(data[swap]->dist_ptr) > *(data[child]->dist_ptr)))
      swap = child;
    if((type == MAX_HEAP && child + 1 <= end && *(data[swap]->dist_ptr) < *(data[child + 1]->dist_ptr)) ||
        (type == MIN_HEAP && child + 1 <= end && *(data[swap]->dist_ptr) > *(data[child + 1]->dist_ptr)))
      swap = child + 1;
    if(swap == root)
      return;
    associated_data *temp = data[root];
    data[root] = data[swap];
    data[root]->heap_idx = root;
    data[swap] = temp;
    data[swap]->heap_idx = swap;
    root = swap;
  }
}

void siftUp(associated_data **data, int start, int end, HEAP_TYPE type) {
  int child = end;
  while(child > start) {
    int parent = idxParent(child);
    if(!((type == MAX_HEAP && *(data[parent]->dist_ptr) < *(data[child]->dist_ptr)) ||
      (type == MIN_HEAP && *(data[parent]->dist_ptr) > *(data[child]->dist_ptr))))
      return;
    associated_data *temp = data[parent];
    data[parent] = data[child];
    data[parent]->heap_idx = parent;
    data[child] = temp;
    data[child]->heap_idx = child;
    child = parent;
  }
}

static int idxParent(int i) {
  return floor((i - 1) /2.0);
}

static int idxLeftChild(int i) {
  return 2 * i + 1;
}

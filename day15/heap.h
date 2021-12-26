typedef enum HEAP_TYPE_ {
  MIN_HEAP,
  MAX_HEAP
} HEAP_TYPE;

typedef enum SIFT_DIR_ {
  SIFT_UP = 1,
  SIFT_DOWN = 2,
} SIFT_DIR;

typedef struct associated_data_{
  int idx;
  int *dist_ptr;
  int heap_idx;
} associated_data;

typedef struct Heap_ {
  int size;
  int capacity;
  associated_data **data;
  HEAP_TYPE type;
} Heap;

// Creation
extern Heap *createHeap(int size, HEAP_TYPE type);
extern Heap *createHeapData(associated_data *data, int size, HEAP_TYPE type);

// Basic
extern associated_data *peek(Heap *heap);
extern Heap *push(Heap *heap, int value);
extern Heap *pushExtra(Heap *heap, int value, void *extra);
extern void set(Heap *heap, int new_val, void *ptr);
extern associated_data *pop(Heap *heap);

// Inspection
extern int isEmpty(Heap *heap);

// Misc
extern void printHeap(Heap *heap);
extern void siftDown(associated_data **data, int start, int end, HEAP_TYPE type);
extern void siftUp(associated_data **data, int start, int end, HEAP_TYPE type);

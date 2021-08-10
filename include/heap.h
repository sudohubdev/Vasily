#define uint32 unsigned int
#define uintptr unsigned int
#define uint8 unsigned char
typedef struct _KHEAPBLOCKBM {
	struct _KHEAPBLOCKBM	*next;
	uint32					size;
	uint32					used;
	uint32					bsize;
	uint32					lfb;
	uintptr					data;
	uint8					*bm;
} KHEAPBLOCKBM;
 
typedef struct _KHEAPBM {
	KHEAPBLOCKBM			*fblock;
} KHEAPBM;
 
void k_heapBMInit(KHEAPBM *heap);
int k_heapBMAddBlock(KHEAPBM *heap, uintptr addr, uint32 size, uint32 bsize);
int k_heapBMAddBlockEx(KHEAPBM *heap, uintptr addr, uint32 size, uint32 bsize, KHEAPBLOCKBM *b, uint8 *bm, uint8 isBMInside);
void *k_heapBMAlloc(KHEAPBM *heap, uint32 size);
void k_heapBMFree(KHEAPBM *heap, void *ptr);
uintptr k_heapBMGetBMSize(uintptr size, uint32 bsize);
void *k_heapBMAllocBound(KHEAPBM *heap, uint32 size, uint32 mask);
void k_heapBMSet(KHEAPBM *heap, uintptr ptr, uintptr size, uint8 rval);
void init_heap();
void* khmalloc(uint32 in);
void* khamalloc(uint32 in);
void khfree(void* in);
void* khrealloc(void* in,unsigned int sz);
void* kharealloc(void* in,unsigned int sz);
 #undef uint32
 #undef uintptr
 #undef uint8


void* dmem_malloc(unsigned int bytes);
void dmem_free(void* p);
void* dmem_cache_dma_malloc(unsigned int bytes);
STATUS dmem_cache_dma_free(void* p);
STATUS dmem_lock(void* p, unsigned int size);
STATUS dmem_unlock(void* virtual, unsigned int size);
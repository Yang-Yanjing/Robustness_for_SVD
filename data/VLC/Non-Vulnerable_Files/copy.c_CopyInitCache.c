#include "copy.h"
int CopyInitCache(copy_cache_t *cache, unsigned width)
{
#ifdef CAN_COMPILE_SSE2
    cache->size = __MAX((width + 0x0f) & ~ 0x0f, 4096);
    cache->buffer = vlc_memalign(16, cache->size);
    if (!cache->buffer)
        return VLC_EGENERIC;
#else
    (void) cache; (void) width;
#endif
    return VLC_SUCCESS;
}

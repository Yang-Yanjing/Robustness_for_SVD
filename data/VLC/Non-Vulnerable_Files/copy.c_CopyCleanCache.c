}
void CopyCleanCache(copy_cache_t *cache)
{
#ifdef CAN_COMPILE_SSE2
    vlc_free(cache->buffer);
    cache->buffer = NULL;
    cache->size   = 0;
#else
    (void) cache;
#endif
}

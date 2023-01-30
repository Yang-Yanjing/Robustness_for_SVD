 */
module_t *CacheFind (module_cache_t *cache, size_t count,
                     const char *path, const struct stat *st)
{
    while (count > 0)
    {
        if (cache->path != NULL
         && !strcmp (cache->path, path)
         && cache->mtime == st->st_mtime
         && cache->size == st->st_size)
       {
            module_t *module = cache->p_module;
            cache->p_module = NULL;
            return module;
       }
       cache++;
       count--;
    }
    return NULL;
}

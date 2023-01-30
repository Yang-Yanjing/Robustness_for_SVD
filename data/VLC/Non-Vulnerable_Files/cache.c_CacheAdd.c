/** Adds entry to the cache */
int CacheAdd (module_cache_t **cachep, size_t *countp,
              const char *path, const struct stat *st, module_t *module)
{
    module_cache_t *cache = *cachep;
    const size_t count = *countp;
    cache = realloc (cache, (count + 1) * sizeof (*cache));
    if (unlikely(cache == NULL))
        return -1;
    *cachep = cache;
    cache += count;
    /* NOTE: strdup() could be avoided, but it would be a bit ugly */
    cache->path = strdup (path);
    cache->mtime = st->st_mtime;
    cache->size = st->st_size;
    cache->p_module = module;
    *countp = count + 1;
    return 0;
}

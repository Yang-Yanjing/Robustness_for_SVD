}
static hls_stream_t *hls_Copy(hls_stream_t *src, const bool b_cp_segments)
{
    assert(src);
    assert(!b_cp_segments); /* FIXME: copying segments is not implemented */
    hls_stream_t *dst = (hls_stream_t *)malloc(sizeof(hls_stream_t));
    if (dst == NULL) return NULL;
    dst->id = src->id;
    dst->bandwidth = src->bandwidth;
    dst->duration = src->duration;
    dst->size = src->size;
    dst->sequence = src->sequence;
    dst->version = src->version;
    dst->b_cache = src->b_cache;
    dst->psz_current_key_path = src->psz_current_key_path ?
                strdup( src->psz_current_key_path ) : NULL;
    dst->url = strdup(src->url);
    if (dst->url == NULL)
    {
        free(dst);
        return NULL;
    }
    if (!b_cp_segments)
        dst->segments = vlc_array_new();
    vlc_mutex_init(&dst->lock);
    return dst;
}

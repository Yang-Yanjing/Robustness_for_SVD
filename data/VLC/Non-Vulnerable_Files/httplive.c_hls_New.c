/* HTTP Live Streaming */
static hls_stream_t *hls_New(vlc_array_t *hls_stream, const int id, const uint64_t bw, const char *uri)
{
    hls_stream_t *hls = (hls_stream_t *)malloc(sizeof(hls_stream_t));
    if (hls == NULL) return NULL;
    hls->id = id;
    hls->bandwidth = bw;
    hls->duration = -1;/* unknown */
    hls->max_segment_length = -1;/* unknown */
    hls->size = 0;
    hls->sequence = 0; /* default is 0 */
    hls->version = 1;  /* default protocol version */
    hls->b_cache = true;
    hls->url = strdup(uri);
    if (hls->url == NULL)
    {
        free(hls);
        return NULL;
    }
    hls->psz_current_key_path = NULL;
    hls->segments = vlc_array_new();
    vlc_array_append(hls_stream, hls);
    vlc_mutex_init(&hls->lock);
    return hls;
}

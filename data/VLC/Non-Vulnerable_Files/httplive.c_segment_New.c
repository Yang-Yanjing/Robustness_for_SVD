/* Segment */
static segment_t *segment_New(hls_stream_t* hls, const int duration, const char *uri)
{
    segment_t *segment = (segment_t *)malloc(sizeof(segment_t));
    if (segment == NULL)
        return NULL;
    segment->duration = duration; /* seconds */
    segment->size = 0; /* bytes */
    segment->sequence = 0;
    segment->bandwidth = 0;
    segment->url = strdup(uri);
    if (segment->url == NULL)
    {
        free(segment);
        return NULL;
    }
    segment->data = NULL;
    vlc_array_append(hls->segments, segment);
    vlc_mutex_init(&segment->lock);
    segment->b_key_loaded = false;
    segment->psz_key_path = NULL;
    if (hls->psz_current_key_path)
        segment->psz_key_path = strdup(hls->psz_current_key_path);
    return segment;
}

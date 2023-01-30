}
static void segment_Free(segment_t *segment)
{
    vlc_mutex_destroy(&segment->lock);
    free(segment->url);
    free(segment->psz_key_path);
    if (segment->data)
        block_Release(segment->data);
    free(segment);
}

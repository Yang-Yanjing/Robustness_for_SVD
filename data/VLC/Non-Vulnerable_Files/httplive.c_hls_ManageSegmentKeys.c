}
static int hls_ManageSegmentKeys(stream_t *s, hls_stream_t *hls)
{
    segment_t   *seg = NULL;
    segment_t   *prev_seg;
    int         count = vlc_array_count(hls->segments);
    for (int i = 0; i < count; i++)
    {
        prev_seg = seg;
        seg = segment_GetSegment(hls, i);
        if (seg == NULL )
            continue;
        if (seg->psz_key_path == NULL)
            continue;   /* No key to load ? continue */
        if (seg->b_key_loaded)
            continue;   /* The key is already loaded */
        /* if the key has not changed, and already available from previous segment,
         * try to copy it, and don't load the key */
        if (prev_seg && prev_seg->b_key_loaded && strcmp(seg->psz_key_path, prev_seg->psz_key_path) == 0)
        {
            memcpy(seg->aes_key, prev_seg->aes_key, AES_BLOCK_SIZE);
            seg->b_key_loaded = true;
            continue;
        }
        if (hls_DownloadSegmentKey(s, seg) != VLC_SUCCESS)
            return VLC_EGENERIC;
       seg->b_key_loaded = true;
    }
    return VLC_SUCCESS;
}

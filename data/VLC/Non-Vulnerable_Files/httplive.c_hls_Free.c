}
static void hls_Free(hls_stream_t *hls)
{
    vlc_mutex_destroy(&hls->lock);
    if (hls->segments)
    {
        for (int n = 0; n < vlc_array_count(hls->segments); n++)
        {
            segment_t *segment = segment_GetSegment(hls, n);
            if (segment) segment_Free(segment);
        }
        vlc_array_destroy(hls->segments);
    }
    free(hls->url);
    free(hls->psz_current_key_path);
    free(hls);
}

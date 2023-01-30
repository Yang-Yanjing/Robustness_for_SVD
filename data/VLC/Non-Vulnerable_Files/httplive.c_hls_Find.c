}
static hls_stream_t *hls_Find(vlc_array_t *hls_stream, hls_stream_t *hls_new)
{
    int count = vlc_array_count(hls_stream);
    for (int n = 0; n < count; n++)
    {
        hls_stream_t *hls = hls_Get(hls_stream, n);
        if (hls)
        {
            /* compare */
            if ((hls->id == hls_new->id) &&
                ((hls->bandwidth == hls_new->bandwidth)||(hls_new->bandwidth==0)))
                return hls;
        }
    }
    return NULL;
}

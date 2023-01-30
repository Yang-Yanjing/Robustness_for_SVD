 ****************************************************************************/
static int BandwidthAdaptation(stream_t *s, int progid, uint64_t *bandwidth)
{
    stream_sys_t *p_sys = s->p_sys;
    int candidate = -1;
    uint64_t bw = *bandwidth;
    uint64_t bw_candidate = 0;
    int count = vlc_array_count(p_sys->hls_stream);
    for (int n = 0; n < count; n++)
    {
        /* Select best bandwidth match */
        hls_stream_t *hls = hls_Get(p_sys->hls_stream, n);
        if (hls == NULL) break;
        /* only consider streams with the same PROGRAM-ID */
        if (hls->id == progid)
        {
            if ((bw >= hls->bandwidth) && (bw_candidate < hls->bandwidth))
            {
                msg_Dbg(s, "candidate %d bandwidth (bits/s) %"PRIu64" >= %"PRIu64,
                         n, bw, hls->bandwidth); /* bits / s */
                bw_candidate = hls->bandwidth;
                candidate = n; /* possible candidate */
            }
        }
    }
    *bandwidth = bw_candidate;
    return candidate;
}

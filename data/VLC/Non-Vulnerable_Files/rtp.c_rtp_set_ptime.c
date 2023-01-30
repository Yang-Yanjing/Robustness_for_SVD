static void
rtp_set_ptime (sout_stream_id_sys_t *id, unsigned ptime_ms, size_t bytes)
{
    /* Samples per second */
    size_t spl = (id->rtp_fmt.clock_rate - 1) * ptime_ms / 1000 + 1;
    bytes *= id->rtp_fmt.channels;
    spl *= bytes;
    if (spl < rtp_mtu (id)) /* MTU is big enough for ptime */
        id->i_mtu = 12 + spl;
    else /* MTU is too small for ptime, align to a sample boundary */
        id->i_mtu = 12 + (((id->i_mtu - 12) / bytes) * bytes);
}

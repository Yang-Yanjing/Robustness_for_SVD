static rtp_source_t *
rtp_source_create (demux_t *demux, const rtp_session_t *session,
                   uint32_t ssrc, uint16_t init_seq)
{
    rtp_source_t *source;
    source = malloc (sizeof (*source) + (sizeof (void *) * session->ptc));
    if (source == NULL)
        return NULL;
    source->ssrc = ssrc;
    source->jitter = 0;
    source->ref_rtp = 0;
    /* TODO: use VLC_TS_0, but VLC does not like negative PTS at the moment */
    source->ref_ntp = UINT64_C (1) << 62;
    source->max_seq = source->bad_seq = init_seq;
    source->last_seq = init_seq - 1;
    source->blocks = NULL;
    /* Initializes all payload */
    for (unsigned i = 0; i < session->ptc; i++)
        source->opaque[i] = session->ptv[i].init (demux);
    msg_Dbg (demux, "added RTP source (%08x)", ssrc);
    return source;
}

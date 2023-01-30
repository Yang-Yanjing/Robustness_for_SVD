static void
rtp_source_destroy (demux_t *demux, const rtp_session_t *session,
                    rtp_source_t *source)
{
    msg_Dbg (demux, "removing RTP source (%08x)", source->ssrc);
    for (unsigned i = 0; i < session->ptc; i++)
        session->ptv[i].destroy (demux, source->opaque[i]);
    block_ChainRelease (source->blocks);
    free (source);
}

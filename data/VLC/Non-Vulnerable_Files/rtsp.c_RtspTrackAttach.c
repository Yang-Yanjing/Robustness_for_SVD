 * initialize with the parameters of the SETUP request */
int RtspTrackAttach( rtsp_stream_t *rtsp, const char *name,
                     rtsp_stream_id_t *id, sout_stream_id_sys_t *sout_id,
                     uint32_t *ssrc, uint16_t *seq_init )
{
    int val = VLC_EGENERIC;
    rtsp_session_t *session;
    vlc_mutex_lock(&rtsp->lock);
    session = RtspClientGet(rtsp, name);
    if (session == NULL)
        goto out;
    rtsp_strack_t *tr = NULL;
    for (int i = 0; i < session->trackc; i++)
    {
        if (session->trackv[i].id == id)
        {
            tr = session->trackv + i;
            break;
        }
    }
    if (tr != NULL)
    {
        tr->sout_id = sout_id;
        tr->rtp_fd = dup_socket(tr->setup_fd);
    }
    else
    {
        /* The track was not SETUP. We still create one because we'll
         * need the sout_id if we set it up later. */
        rtsp_strack_t track = { .id = id, .sout_id = sout_id,
                                .setup_fd = -1, .rtp_fd = -1 };
        vlc_rand_bytes (&track.seq_init, sizeof (track.seq_init));
        vlc_rand_bytes (&track.ssrc, sizeof (track.ssrc));
        INSERT_ELEM(session->trackv, session->trackc, session->trackc, track);
        tr = session->trackv + session->trackc - 1;
    }
    *ssrc = ntohl(tr->ssrc);
    *seq_init = tr->seq_init;
    if (tr->rtp_fd != -1)
    {
        uint16_t seq;
        rtp_add_sink(tr->sout_id, tr->rtp_fd, false, &seq);
        /* To avoid race conditions, sout_id->i_seq_sent_next must
         * be set here and now. Make sure the caller did its job
         * properly when passing seq_init. */
        assert(tr->seq_init == seq);
    }
    val = VLC_SUCCESS;
out:
    vlc_mutex_unlock(&rtsp->lock);
    return val;
}

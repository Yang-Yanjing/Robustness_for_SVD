/* Remove references to the RTP id when it is stopped */
void RtspTrackDetach( rtsp_stream_t *rtsp, const char *name,
                      sout_stream_id_sys_t *sout_id )
{
    rtsp_session_t *session;
    vlc_mutex_lock(&rtsp->lock);
    session = RtspClientGet(rtsp, name);
    if (session == NULL)
        goto out;
    for (int i = 0; i < session->trackc; i++)
    {
        rtsp_strack_t *tr = session->trackv + i;
        if (tr->sout_id == sout_id)
        {
            if (tr->setup_fd == -1)
            {
                /* No (more) SETUP information: better get rid of the
                 * track so that we can have new random ssrc and
                 * seq_init next time. */
                REMOVE_ELEM( session->trackv, session->trackc, i );
                break;
            }
            /* We keep the SETUP information of the track, but stop it */
            if (tr->rtp_fd != -1)
            {
                rtp_del_sink(tr->sout_id, tr->rtp_fd);
                tr->rtp_fd = -1;
            }
            tr->sout_id = NULL;
            break;
        }
    }
out:
    vlc_mutex_unlock(&rtsp->lock);
}

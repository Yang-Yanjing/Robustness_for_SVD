rtp_session_t *
rtp_session_create (demux_t *demux)
{
    rtp_session_t *session = malloc (sizeof (*session));
    if (session == NULL)
        return NULL;
    session->srcv = NULL;
    session->srcc = 0;
    session->ptc = 0;
    session->ptv = NULL;
    (void)demux;
    return session;
}

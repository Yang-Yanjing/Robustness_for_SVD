/** rtsp must be locked */
static void RtspClientAlive( rtsp_session_t *session )
{
    if (session->stream->timeout <= 0)
        return;
    session->last_seen = mdate();
    RtspUpdateTimer(session->stream);
}

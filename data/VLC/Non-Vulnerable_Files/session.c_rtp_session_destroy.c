 */
void rtp_session_destroy (demux_t *demux, rtp_session_t *session)
{
    for (unsigned i = 0; i < session->srcc; i++)
        rtp_source_destroy (demux, session, session->srcv[i]);
    free (session->srcv);
    free (session->ptv);
    free (session);
    (void)demux;
}

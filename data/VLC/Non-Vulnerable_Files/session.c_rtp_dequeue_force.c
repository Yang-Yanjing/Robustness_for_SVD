 */
void rtp_dequeue_force (demux_t *demux, const rtp_session_t *session)
{
    for (unsigned i = 0, max = session->srcc; i < max; i++)
    {
        rtp_source_t *src = session->srcv[i];
        block_t *block;
        while (((block = src->blocks)) != NULL)
            rtp_decode (demux, session, src);
    }
}

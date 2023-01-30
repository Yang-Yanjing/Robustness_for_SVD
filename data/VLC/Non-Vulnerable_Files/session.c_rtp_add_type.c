 */
int rtp_add_type (demux_t *demux, rtp_session_t *ses, const rtp_pt_t *pt)
{
    if (ses->srcc > 0)
    {
        msg_Err (demux, "cannot change RTP payload formats during session");
        return EINVAL;
    }
    rtp_pt_t *ppt = realloc (ses->ptv, (ses->ptc + 1) * sizeof (rtp_pt_t));
    if (ppt == NULL)
        return ENOMEM;
    ses->ptv = ppt;
    ppt += ses->ptc++;
    ppt->init = pt->init ? pt->init : no_init;
    ppt->destroy = pt->destroy ? pt->destroy : no_destroy;
    ppt->decode = pt->decode ? pt->decode : no_decode;
    ppt->frequency = pt->frequency;
    ppt->number = pt->number;
    msg_Dbg (demux, "added payload type %"PRIu8" (f = %"PRIu32" Hz)",
             ppt->number, ppt->frequency);
    assert (ppt->frequency > 0); /* SIGFPE! */
    (void)demux;
    return 0;
}

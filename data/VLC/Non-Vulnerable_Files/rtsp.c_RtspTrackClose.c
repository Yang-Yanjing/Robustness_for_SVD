/** rtsp must be locked */
static void RtspTrackClose( rtsp_strack_t *tr )
{
    if (tr->setup_fd != -1)
    {
        if (tr->rtp_fd != -1)
        {
            rtp_del_sink(tr->sout_id, tr->rtp_fd);
            tr->rtp_fd = -1;
        }
        net_Close(tr->setup_fd);
        tr->setup_fd = -1;
    }
}

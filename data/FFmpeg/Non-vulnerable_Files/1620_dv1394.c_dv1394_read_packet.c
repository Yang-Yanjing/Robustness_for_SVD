static int dv1394_read_packet(AVFormatContext *context, AVPacket *pkt)
{
    struct dv1394_data *dv = context->priv_data;
    int size;
    size = avpriv_dv_get_packet(dv->dv_demux, pkt);
    if (size > 0)
        return size;
    if (!dv->avail) {
        struct dv1394_status s;
        struct pollfd p;
        if (dv->done) {
            
            if (ioctl(dv->fd, DV1394_RECEIVE_FRAMES, dv->done) < 0) {
                


                av_log(context, AV_LOG_ERROR, "DV1394: Ring buffer overflow. Reseting ..\n");
                dv1394_reset(dv);
                dv1394_start(dv);
            }
            dv->done = 0;
        }
        
restart_poll:
        p.fd = dv->fd;
        p.events = POLLIN | POLLERR | POLLHUP;
        if (poll(&p, 1, -1) < 0) {
            if (errno == EAGAIN || errno == EINTR)
                goto restart_poll;
            av_log(context, AV_LOG_ERROR, "Poll failed: %s\n", strerror(errno));
            return AVERROR(EIO);
        }
        if (ioctl(dv->fd, DV1394_GET_STATUS, &s) < 0) {
            av_log(context, AV_LOG_ERROR, "Failed to get status: %s\n", strerror(errno));
            return AVERROR(EIO);
        }
        av_log(context, AV_LOG_TRACE, "DV1394: status\n"
                "\tactive_frame\t%d\n"
                "\tfirst_clear_frame\t%d\n"
                "\tn_clear_frames\t%d\n"
                "\tdropped_frames\t%d\n",
                s.active_frame, s.first_clear_frame,
                s.n_clear_frames, s.dropped_frames);
        dv->avail = s.n_clear_frames;
        dv->index = s.first_clear_frame;
        dv->done  = 0;
        if (s.dropped_frames) {
            av_log(context, AV_LOG_ERROR, "DV1394: Frame drop detected (%d). Reseting ..\n",
                    s.dropped_frames);
            dv1394_reset(dv);
            dv1394_start(dv);
        }
    }
    av_log(context, AV_LOG_TRACE, "index %d, avail %d, done %d\n", dv->index, dv->avail,
            dv->done);
    size = avpriv_dv_produce_packet(dv->dv_demux, pkt,
                             dv->ring + (dv->index * DV1394_PAL_FRAME_SIZE),
                             DV1394_PAL_FRAME_SIZE, -1);
    dv->index = (dv->index + 1) % DV1394_RING_FRAMES;
    dv->done++; dv->avail--;
    return size;
}

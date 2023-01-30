static int dv1394_close(AVFormatContext * context)
{
    struct dv1394_data *dv = context->priv_data;
    
    if (ioctl(dv->fd, DV1394_SHUTDOWN, 0) < 0)
        av_log(context, AV_LOG_ERROR, "Failed to shutdown DV1394: %s\n", strerror(errno));
    
    if (munmap(dv->ring, DV1394_NTSC_FRAME_SIZE * DV1394_RING_FRAMES) < 0)
        av_log(context, AV_LOG_ERROR, "Failed to munmap DV1394 ring buffer: %s\n", strerror(errno));
    close(dv->fd);
    av_freep(&dv->dv_demux);
    return 0;
}

static int dv1394_read_header(AVFormatContext * context)
{
    struct dv1394_data *dv = context->priv_data;
    dv->dv_demux = avpriv_dv_init_demux(context);
    if (!dv->dv_demux)
        goto failed;
    
    dv->fd = avpriv_open(context->filename, O_RDONLY);
    if (dv->fd < 0) {
        av_log(context, AV_LOG_ERROR, "Failed to open DV interface: %s\n", strerror(errno));
        goto failed;
    }
    if (dv1394_reset(dv) < 0) {
        av_log(context, AV_LOG_ERROR, "Failed to initialize DV interface: %s\n", strerror(errno));
        goto failed;
    }
    dv->ring = mmap(NULL, DV1394_PAL_FRAME_SIZE * DV1394_RING_FRAMES,
                    PROT_READ, MAP_PRIVATE, dv->fd, 0);
    if (dv->ring == MAP_FAILED) {
        av_log(context, AV_LOG_ERROR, "Failed to mmap DV ring buffer: %s\n", strerror(errno));
        goto failed;
    }
    if (dv1394_start(dv) < 0)
        goto failed;
    return 0;
failed:
    close(dv->fd);
    return AVERROR(EIO);
}

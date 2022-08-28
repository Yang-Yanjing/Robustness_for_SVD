static int dv1394_start(struct dv1394_data *dv)
{
    
    if (ioctl(dv->fd, DV1394_START_RECEIVE, 0) < 0) {
        av_log(NULL, AV_LOG_ERROR, "Failed to start receiver: %s\n", strerror(errno));
        return -1;
    }
    return 0;
}

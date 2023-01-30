static int dv1394_reset(struct dv1394_data *dv)
{
    struct dv1394_init init;
    init.channel     = dv->channel;
    init.api_version = DV1394_API_VERSION;
    init.n_frames    = DV1394_RING_FRAMES;
    init.format      = DV1394_PAL;
    if (ioctl(dv->fd, DV1394_INIT, &init) < 0)
        return -1;
    dv->avail  = dv->done = 0;
    return 0;
}

static int init_MP4DescrParseContext(MP4DescrParseContext *d, AVFormatContext *s,
                                     const uint8_t *buf, unsigned size,
                                     Mp4Descr *descr, int max_descr_count)
{
    int ret;
    if (size > (1 << 30))
        return AVERROR_INVALIDDATA;
    if ((ret = ffio_init_context(&d->pb, (unsigned char *)buf, size, 0,
                                 NULL, NULL, NULL, NULL)) < 0)
        return ret;
    d->s               = s;
    d->level           = 0;
    d->descr_count     = 0;
    d->descr           = descr;
    d->active_descr    = NULL;
    d->max_descr_count = max_descr_count;
    return 0;
}

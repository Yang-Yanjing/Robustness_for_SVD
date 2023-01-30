}
static picture_t *VideoExport_Filter(filter_t *filter, picture_t *src)
{
    if (unlikely(src->context == NULL))
    {
        msg_Err(filter, "corrupt VDPAU video surface %p", src);
        picture_Release(src);
        return NULL;
    }
    picture_t *dst = filter_NewPicture(filter);
    if (dst == NULL)
        return NULL;
    return VideoExport(filter, src, dst);
}

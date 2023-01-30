}
static picture_t *Filter(filter_t *p_filter, picture_t *p_pic)
{
    filter_sys_t *p_sys = p_filter->p_sys;
    if (!p_pic)
        return NULL;
    picture_t *p_outpic = filter_NewPicture(p_filter);
    if (!p_outpic)
    {
        picture_Release(p_pic);
        return NULL;
    }
    switch (p_pic->format.i_chroma)
    {
        case VLC_CODEC_I420:
        case VLC_CODEC_J420:
        case VLC_CODEC_YV12:
            combine_side_by_side_yuv420(p_pic, p_outpic,
                                        p_sys->left, p_sys->right);
            break;
        default:
            msg_Warn(p_filter, "Unsupported input chroma (%4.4s)",
                     (char*)&(p_pic->format.i_chroma));
            picture_Release(p_pic);
            return NULL;
    }
    return CopyInfoAndRelease(p_outpic, p_pic);
}

int av_frame_copy_props(AVFrame *dst, const AVFrame *src)
{
    return frame_copy_props(dst, src, 1);
}

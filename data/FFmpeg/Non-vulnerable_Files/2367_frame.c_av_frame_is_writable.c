int av_frame_is_writable(AVFrame *frame)
{
    int i, ret = 1;
    
    if (!frame->buf[0])
        return 0;
    for (i = 0; i < FF_ARRAY_ELEMS(frame->buf); i++)
        if (frame->buf[i])
            ret &= !!av_buffer_is_writable(frame->buf[i]);
    for (i = 0; i < frame->nb_extended_buf; i++)
        ret &= !!av_buffer_is_writable(frame->extended_buf[i]);
    return ret;
}

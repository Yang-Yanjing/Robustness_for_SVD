static int frame_needs_copy(PadContext *s, AVFrame *frame)
{
    int i;
    if (!av_frame_is_writable(frame))
        return 1;
    for (i = 0; i < 4 && frame->buf[i]; i++)
        if (buffer_needs_copy(s, frame, frame->buf[i]))
            return 1;
    return 0;
}

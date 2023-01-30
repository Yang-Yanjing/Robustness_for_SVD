static int opengl_write_frame(AVFormatContext *h, int stream_index,
                              AVFrame **frame, unsigned flags)
{
    if ((flags & AV_WRITE_UNCODED_FRAME_QUERY))
        return 0;
    return opengl_draw(h, *frame, 0, 0);
}

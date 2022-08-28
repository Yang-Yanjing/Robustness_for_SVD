static int xv_write_frame(AVFormatContext *s, int stream_index, AVFrame **frame,
                          unsigned flags)
{
    
    if ((flags & AV_WRITE_UNCODED_FRAME_QUERY))
        return 0;
    return write_picture(s, (AVPicture *)*frame);
}

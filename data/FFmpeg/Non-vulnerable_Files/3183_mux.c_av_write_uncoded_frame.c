int av_write_uncoded_frame(AVFormatContext *s, int stream_index,
                           AVFrame *frame)
{
    return av_write_uncoded_frame_internal(s, stream_index, frame, 0);
}

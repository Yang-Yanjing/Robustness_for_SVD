int av_write_uncoded_frame_query(AVFormatContext *s, int stream_index)
{
    av_assert0(s->oformat);
    if (!s->oformat->write_uncoded_frame)
        return AVERROR(ENOSYS);
    return s->oformat->write_uncoded_frame(s, stream_index, NULL,
                                           AV_WRITE_UNCODED_FRAME_QUERY);
}

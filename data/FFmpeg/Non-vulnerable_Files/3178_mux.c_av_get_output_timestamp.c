int av_get_output_timestamp(struct AVFormatContext *s, int stream,
                            int64_t *dts, int64_t *wall)
{
    if (!s->oformat || !s->oformat->get_output_timestamp)
        return AVERROR(ENOSYS);
    s->oformat->get_output_timestamp(s, stream, dts, wall);
    return 0;
}

static int flv_get_extradata(AVFormatContext *s, AVStream *st, int size)
{
    av_freep(&st->codec->extradata);
    if (ff_get_extradata(st->codec, s->pb, size) < 0)
        return AVERROR(ENOMEM);
    return 0;
}

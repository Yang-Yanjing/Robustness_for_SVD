static int init_pts(AVFormatContext *s)
{
    int i;
    AVStream *st;
    
    for (i = 0; i < s->nb_streams; i++) {
        int64_t den = AV_NOPTS_VALUE;
        st = s->streams[i];
        switch (st->codec->codec_type) {
        case AVMEDIA_TYPE_AUDIO:
            den = (int64_t)st->time_base.num * st->codec->sample_rate;
            break;
        case AVMEDIA_TYPE_VIDEO:
            den = (int64_t)st->time_base.num * st->codec->time_base.den;
            break;
        default:
            break;
        }
        if (!st->priv_pts)
            st->priv_pts = av_mallocz(sizeof(*st->priv_pts));
        if (!st->priv_pts)
            return AVERROR(ENOMEM);
        if (den != AV_NOPTS_VALUE) {
            if (den <= 0)
                return AVERROR_INVALIDDATA;
            frac_init(st->priv_pts, 0, 0, den);
        }
    }
    return 0;
}

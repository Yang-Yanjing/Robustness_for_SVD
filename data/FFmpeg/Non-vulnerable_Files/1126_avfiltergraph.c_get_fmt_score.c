static int get_fmt_score(enum AVSampleFormat dst_fmt, enum AVSampleFormat src_fmt)
{
    int score = 0;
    if (av_sample_fmt_is_planar(dst_fmt) != av_sample_fmt_is_planar(src_fmt))
        score ++;
    if (av_get_bytes_per_sample(dst_fmt) < av_get_bytes_per_sample(src_fmt)) {
        score += 100 * (av_get_bytes_per_sample(src_fmt) - av_get_bytes_per_sample(dst_fmt));
    }else
        score += 10  * (av_get_bytes_per_sample(dst_fmt) - av_get_bytes_per_sample(src_fmt));
    if (av_get_packed_sample_fmt(dst_fmt) == AV_SAMPLE_FMT_S32 &&
        av_get_packed_sample_fmt(src_fmt) == AV_SAMPLE_FMT_FLT)
        score += 20;
    if (av_get_packed_sample_fmt(dst_fmt) == AV_SAMPLE_FMT_FLT &&
        av_get_packed_sample_fmt(src_fmt) == AV_SAMPLE_FMT_S32)
        score += 2;
    return score;
}

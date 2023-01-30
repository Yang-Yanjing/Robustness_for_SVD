static enum AVSampleFormat find_best_sample_fmt_of_2(enum AVSampleFormat dst_fmt1, enum AVSampleFormat dst_fmt2,
                                                     enum AVSampleFormat src_fmt)
{
    int score1, score2;
    score1 = get_fmt_score(dst_fmt1, src_fmt);
    score2 = get_fmt_score(dst_fmt2, src_fmt);
    return score1 < score2 ? dst_fmt1 : dst_fmt2;
}

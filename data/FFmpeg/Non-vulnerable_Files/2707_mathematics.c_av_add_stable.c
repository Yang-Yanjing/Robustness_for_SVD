int64_t av_add_stable(AVRational ts_tb, int64_t ts, AVRational inc_tb, int64_t inc)
{
    int64_t m, d;
    if (inc != 1)
        inc_tb = av_mul_q(inc_tb, (AVRational) {inc, 1});
    m = inc_tb.num * (int64_t)ts_tb.den;
    d = inc_tb.den * (int64_t)ts_tb.num;
    if (m % d == 0)
        return ts + m / d;
    if (m < d)
        return ts;
    {
        int64_t old = av_rescale_q(ts, ts_tb, inc_tb);
        int64_t old_ts = av_rescale_q(old, inc_tb, ts_tb);
        return av_rescale_q(old + 1, inc_tb, ts_tb) + (ts - old_ts);
    }
}

int av_nearer_q(AVRational q, AVRational q1, AVRational q2)
{
    
    int64_t a = q1.num * (int64_t)q2.den + q2.num * (int64_t)q1.den;
    int64_t b = 2 * (int64_t)q1.den * q2.den;
    
    int64_t x_up = av_rescale_rnd(a, q.den, b, AV_ROUND_UP);
    
    int64_t x_down = av_rescale_rnd(a, q.den, b, AV_ROUND_DOWN);
    return ((x_up > q.num) - (x_down < q.num)) * av_cmp_q(q2, q1);
}

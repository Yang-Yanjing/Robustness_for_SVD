AVRational av_div_q(AVRational b, AVRational c)
{
    return av_mul_q(b, (AVRational) { c.den, c.num });
}

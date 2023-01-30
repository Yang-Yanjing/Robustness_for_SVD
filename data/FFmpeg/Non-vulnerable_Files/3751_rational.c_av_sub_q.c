AVRational av_sub_q(AVRational b, AVRational c)
{
    return av_add_q(b, (AVRational) { -c.num, c.den });
}

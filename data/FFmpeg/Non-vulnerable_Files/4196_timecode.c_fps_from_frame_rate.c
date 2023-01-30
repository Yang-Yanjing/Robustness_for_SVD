static int fps_from_frame_rate(AVRational rate)
{
    if (!rate.den || !rate.num)
        return -1;
    return (rate.num + rate.den/2) / rate.den;
}

static double get_f64l(uint8_t *p)
{
    return av_int2double(AV_RL64(p));
}

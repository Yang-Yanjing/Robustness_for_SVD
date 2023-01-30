}
static float AdjustRateFine( vlc_object_t *p_obj, const int i_dir )
{
    const float f_rate_min = (float)INPUT_RATE_DEFAULT / INPUT_RATE_MAX;
    const float f_rate_max = (float)INPUT_RATE_DEFAULT / INPUT_RATE_MIN;
    float f_rate = var_GetFloat( p_obj, "rate" );
    int i_sign = f_rate < 0 ? -1 : 1;
    f_rate = floor( fabs(f_rate) / 0.1 + i_dir + 0.05 ) * 0.1;
    if( f_rate < f_rate_min )
        f_rate = f_rate_min;
    else if( f_rate > f_rate_max )
        f_rate = f_rate_max;
    f_rate *= i_sign;
    return f_rate;
}

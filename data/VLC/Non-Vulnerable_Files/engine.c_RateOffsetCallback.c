}
static int RateOffsetCallback( vlc_object_t *obj, char const *psz_cmd,
                               vlc_value_t oldval, vlc_value_t newval, void *p_data )
{
    playlist_t *p_playlist = (playlist_t *)obj;
    VLC_UNUSED(oldval); VLC_UNUSED(p_data); VLC_UNUSED(newval);
    static const float pf_rate[] = {
        1.0/64, 1.0/32, 1.0/16, 1.0/8, 1.0/4, 1.0/3, 1.0/2, 2.0/3,
        1.0/1,
        3.0/2, 2.0/1, 3.0/1, 4.0/1, 8.0/1, 16.0/1, 32.0/1, 64.0/1,
    };
    const size_t i_rate_count = sizeof(pf_rate)/sizeof(*pf_rate);
    float f_rate;
    struct input_thread_t *input;
    PL_LOCK;
    input = pl_priv( p_playlist )->p_input;
    f_rate = var_GetFloat( input ? (vlc_object_t *)input : obj, "rate" );
    PL_UNLOCK;
    if( !strcmp( psz_cmd, "rate-faster" ) )
    {
        /* compensate for input rounding errors */
        float r = f_rate * 1.1;
        for( size_t i = 0; i < i_rate_count; i++ )
            if( r < pf_rate[i] )
            {
                f_rate = pf_rate[i];
                break;
            }
    }
    else
    {
        /* compensate for input rounding errors */
        float r = f_rate * .9;
        for( size_t i = 1; i < i_rate_count; i++ )
            if( r <= pf_rate[i] )
            {
                f_rate = pf_rate[i - 1];
                break;
            }
    }
    var_SetFloat( p_playlist, "rate", f_rate );
    return VLC_SUCCESS;
}

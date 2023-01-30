}
static int alphaCallback( vlc_object_t *p_this, char const *psz_var,
                          vlc_value_t oldval, vlc_value_t newval,
                          void *p_data )
{
    VLC_UNUSED(p_this); VLC_UNUSED(oldval); VLC_UNUSED(psz_var);
    sout_stream_t *p_stream = (sout_stream_t *)p_data;
    sout_stream_sys_t *p_sys = p_stream->p_sys;
    if( p_sys->p_es )
        p_sys->p_es->i_alpha = newval.i_int;
    return VLC_SUCCESS;
}

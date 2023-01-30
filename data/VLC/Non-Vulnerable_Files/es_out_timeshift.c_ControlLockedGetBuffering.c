}
static int ControlLockedGetBuffering( es_out_t *p_out, bool *pb_buffering )
{
    es_out_sys_t *p_sys = p_out->p_sys;
    if( p_sys->b_delayed )
        *pb_buffering = true;
    else
        *pb_buffering = es_out_GetBuffering( p_sys->p_out );
    return VLC_SUCCESS;
}

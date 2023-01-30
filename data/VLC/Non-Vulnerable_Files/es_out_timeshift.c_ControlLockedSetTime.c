}
static int ControlLockedSetTime( es_out_t *p_out, mtime_t i_date )
{
    es_out_sys_t *p_sys = p_out->p_sys;
    if( !p_sys->b_delayed )
        return es_out_SetTime( p_sys->p_out, i_date );
    /* TODO */
    msg_Err( p_sys->p_input, "EsOutTimeshift does not yet support time change" );
    return VLC_EGENERIC;
}

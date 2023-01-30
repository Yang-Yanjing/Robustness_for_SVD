}
static int EventIntf( vlc_object_t *p_input, char const *psz_var,
                      vlc_value_t oldval, vlc_value_t val, void *p_data )
{
    demux_t *p_demux = p_data;
    demux_sys_t *p_sys = p_demux->p_sys;
    if (val.i_int == INPUT_EVENT_VOUT)
    {
        if( p_sys->p_vout != NULL )
        {
            var_DelCallback( p_sys->p_vout, "mouse-moved", EventMouse, p_demux );
            var_DelCallback( p_sys->p_vout, "mouse-clicked", EventMouse, p_demux );
            vlc_object_release( p_sys->p_vout );
        }
        p_sys->p_vout = input_GetVout( (input_thread_t *)p_input );
        if( p_sys->p_vout != NULL )
        {
            var_AddCallback( p_sys->p_vout, "mouse-moved", EventMouse, p_demux );
            var_AddCallback( p_sys->p_vout, "mouse-clicked", EventMouse, p_demux );
        }
    }
    (void) psz_var; (void) oldval;
    return VLC_SUCCESS;
}

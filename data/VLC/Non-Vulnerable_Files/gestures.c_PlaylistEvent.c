}
static int PlaylistEvent( vlc_object_t *p_this, char const *psz_var,
                          vlc_value_t oldval, vlc_value_t val, void *p_data )
{
    intf_thread_t *p_intf = p_data;
    intf_sys_t *p_sys = p_intf->p_sys;
    input_thread_t *p_input = val.p_address;
    (void) p_this; (void) psz_var; (void) oldval;
    var_AddCallback( p_input, "intf-event", InputEvent, p_intf );
    assert( p_sys->p_input == NULL );
    p_sys->p_input = vlc_object_hold( p_input );
    return VLC_SUCCESS;
}

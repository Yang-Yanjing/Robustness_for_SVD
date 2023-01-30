}
static int InputEvent( vlc_object_t *p_this, char const *psz_var,
                       vlc_value_t oldval, vlc_value_t val, void *p_data )
{
    input_thread_t *p_input = (input_thread_t *)p_this;
    intf_thread_t *p_intf = p_data;
    intf_sys_t *p_sys = p_intf->p_sys;
    (void) psz_var; (void) oldval;
    switch( val.i_int )
    {
      case INPUT_EVENT_DEAD:
        vlc_object_release( p_input );
        p_sys->p_input = NULL; /* FIXME: locking!! */
        break;
      case INPUT_EVENT_VOUT:
        /* intf-event is serialized against itself and is the sole user of
         * p_sys->p_vout. So there is no need to acquire the lock currently. */
        if( p_sys->p_vout != NULL )
        {   /* /!\ Beware of lock inversion with var_DelCallback() /!\ */
            var_DelCallback( p_sys->p_vout, "mouse-moved", MovedEvent,
                             p_intf );
            var_DelCallback( p_sys->p_vout, "mouse-button-down", ButtonEvent,
                             p_intf );
            vlc_object_release( p_sys->p_vout );
        }
        p_sys->p_vout = input_GetVout( p_input );
        if( p_sys->p_vout != NULL )
        {
            var_AddCallback( p_sys->p_vout, "mouse-moved", MovedEvent,
                             p_intf );
            var_AddCallback( p_sys->p_vout, "mouse-button-down", ButtonEvent,
                             p_intf );
        }
        break;
    }
    return VLC_SUCCESS;
}

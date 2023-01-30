 *****************************************************************************/
DBUS_SIGNAL( SeekedSignal )
{
    SIGNAL_INIT( DBUS_MPRIS_PLAYER_INTERFACE,
                 DBUS_MPRIS_OBJECT_PATH,
                 "Seeked" );
    OUT_ARGUMENTS;
    dbus_int64_t i_pos = 0;
    intf_thread_t *p_intf = (intf_thread_t*) p_data;
    input_thread_t *p_input = pl_CurrentInput( p_intf );
    if( p_input )
    {
        i_pos = var_GetTime( p_input, "time" );
        vlc_object_release( p_input );
    }
    ADD_INT64( &i_pos );
    SIGNAL_SEND;
}

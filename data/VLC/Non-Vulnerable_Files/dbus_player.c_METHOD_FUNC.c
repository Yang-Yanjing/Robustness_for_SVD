#undef ADD_PROPERTY
#define METHOD_FUNC( interface, method, function ) \
    else if( dbus_message_is_method_call( p_from, interface, method ) )\
        return function( p_conn, p_from, p_this )
DBusHandlerResult
handle_player ( DBusConnection *p_conn, DBusMessage *p_from, void *p_this )
{
    if(0);
    METHOD_FUNC( DBUS_INTERFACE_PROPERTIES,   "Get",        GetProperty );
    METHOD_FUNC( DBUS_INTERFACE_PROPERTIES,   "Set",        SetProperty );
    METHOD_FUNC( DBUS_INTERFACE_PROPERTIES,   "GetAll",     GetAllProperties );
    /* here D-Bus method names are associated to an handler */
    METHOD_FUNC( DBUS_MPRIS_PLAYER_INTERFACE, "Previous",     Prev );
    METHOD_FUNC( DBUS_MPRIS_PLAYER_INTERFACE, "Next",         Next );
    METHOD_FUNC( DBUS_MPRIS_PLAYER_INTERFACE, "Stop",         Stop );
    METHOD_FUNC( DBUS_MPRIS_PLAYER_INTERFACE, "Seek",         Seek );
    METHOD_FUNC( DBUS_MPRIS_PLAYER_INTERFACE, "Play",         Play );
    METHOD_FUNC( DBUS_MPRIS_PLAYER_INTERFACE, "Pause",        Pause );
    METHOD_FUNC( DBUS_MPRIS_PLAYER_INTERFACE, "PlayPause",    PlayPause );
    METHOD_FUNC( DBUS_MPRIS_PLAYER_INTERFACE, "OpenUri",      OpenUri );
    METHOD_FUNC( DBUS_MPRIS_PLAYER_INTERFACE, "SetPosition",  SetPosition );
    return DBUS_HANDLER_RESULT_NOT_YET_HANDLED;
}

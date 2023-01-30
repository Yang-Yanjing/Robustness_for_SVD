#undef ADD_PROPERTY
#define METHOD_FUNC( interface, method, function ) \
    else if( dbus_message_is_method_call( p_from, interface, method ) )\
        return function( p_conn, p_from, p_this )
DBusHandlerResult
handle_tracklist ( DBusConnection *p_conn, DBusMessage *p_from, void *p_this )
{
    if(0);
    METHOD_FUNC( DBUS_INTERFACE_PROPERTIES, "Get",    GetProperty );
    METHOD_FUNC( DBUS_INTERFACE_PROPERTIES, "GetAll", GetAllProperties );
    /* here D-Bus method names are associated to an handler */
    METHOD_FUNC( DBUS_MPRIS_TRACKLIST_INTERFACE, "GoTo",        GoTo );
    METHOD_FUNC( DBUS_MPRIS_TRACKLIST_INTERFACE, "AddTrack",    AddTrack );
    METHOD_FUNC( DBUS_MPRIS_TRACKLIST_INTERFACE, "RemoveTrack", RemoveTrack );
    METHOD_FUNC( DBUS_MPRIS_TRACKLIST_INTERFACE, "GetTracksMetadata",
                                                  GetTracksMetadata );
    return DBUS_HANDLER_RESULT_NOT_YET_HANDLED;
}

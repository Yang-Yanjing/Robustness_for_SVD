DBusHandlerResult
handle_root ( DBusConnection *p_conn, DBusMessage *p_from, void *p_this )
{
    METHOD_MAPPING_BEGIN
    METHOD_FUNC( DBUS_INTERFACE_PROPERTIES, "Get",          GetProperty );
    METHOD_FUNC( DBUS_INTERFACE_PROPERTIES, "Set",          SetProperty );
    METHOD_FUNC( DBUS_INTERFACE_PROPERTIES, "GetAll",       GetAllProperties );
    METHOD_FUNC( DBUS_MPRIS_ROOT_INTERFACE, "Quit",         Quit );
    METHOD_FUNC( DBUS_MPRIS_ROOT_INTERFACE, "Raise",        Raise );
    METHOD_MAPPING_END
}

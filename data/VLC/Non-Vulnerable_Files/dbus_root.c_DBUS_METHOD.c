    }
DBUS_METHOD( GetAllProperties )
{
    REPLY_INIT;
    OUT_ARGUMENTS;
    DBusError error;
    DBusMessageIter dict;
    char *const psz_interface_name = NULL;
    dbus_error_init( &error );
    dbus_message_get_args( p_from, &error,
            DBUS_TYPE_STRING, &psz_interface_name,
            DBUS_TYPE_INVALID );
    if( dbus_error_is_set( &error ) )
    {
        msg_Err( (vlc_object_t*) p_this, "D-Bus message reading : %s",
                                         error.message );
        dbus_error_free( &error );
        return DBUS_HANDLER_RESULT_NOT_YET_HANDLED;
    }
    msg_Dbg( (vlc_object_t*) p_this, "Getting All properties" );
    if( !dbus_message_iter_open_container( &args, DBUS_TYPE_ARRAY, "{sv}", &dict ) )
        return DBUS_HANDLER_RESULT_NEED_MEMORY;
    ADD_PROPERTY( Identity,            "s"  );
    ADD_PROPERTY( DesktopEntry,        "s"  );
    ADD_PROPERTY( SupportedMimeTypes,  "as" );
    ADD_PROPERTY( SupportedUriSchemes, "as" );
    ADD_PROPERTY( HasTrackList,        "b"  );
    ADD_PROPERTY( CanQuit,             "b"  );
    ADD_PROPERTY( CanSetFullscreen,    "b"  );
    ADD_PROPERTY( Fullscreen,          "b"  );
    ADD_PROPERTY( CanRaise,            "b"  );
    if( !dbus_message_iter_close_container( &args, &dict ))
        return DBUS_HANDLER_RESULT_NEED_MEMORY;
    REPLY_SEND;
}

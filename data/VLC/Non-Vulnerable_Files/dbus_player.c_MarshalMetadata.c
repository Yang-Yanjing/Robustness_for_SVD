static int
MarshalMetadata( intf_thread_t *p_intf, DBusMessageIter *container )
{
    DBusMessageIter a;
    input_thread_t *p_input = pl_CurrentInput( p_intf );
    input_item_t   *p_item  = NULL;
    if( p_input != NULL )
    {
        p_item = input_GetItem( p_input );
        if( p_item )
        {
            int result = GetInputMeta( p_item, container );
            if (result != VLC_SUCCESS)
            {
                vlc_object_release( (vlc_object_t*) p_input );
                return result;
            }
        }
        vlc_object_release( (vlc_object_t*) p_input );
    }
    if (!p_item)
    {
        // avoid breaking the type marshalling
        if( !dbus_message_iter_open_container( container, DBUS_TYPE_ARRAY, "{sv}", &a ) ||
              !dbus_message_iter_close_container( container, &a ) )
            return VLC_ENOMEM;
    }
    return VLC_SUCCESS;
}

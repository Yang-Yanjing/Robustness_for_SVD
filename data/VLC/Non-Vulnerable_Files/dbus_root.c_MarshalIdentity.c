static int
MarshalIdentity( intf_thread_t *p_intf, DBusMessageIter *container )
{
    VLC_UNUSED( p_intf );
    const char *psz_id = _("VLC media player");
    if (!dbus_message_iter_append_basic( container, DBUS_TYPE_STRING, &psz_id ))
        return VLC_ENOMEM;
    return VLC_SUCCESS;
}

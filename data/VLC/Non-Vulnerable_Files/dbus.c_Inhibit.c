};
static void Inhibit(vlc_inhibit_t *ih, unsigned flags)
{
    vlc_inhibit_sys_t *sys = ih->p_sys;
    enum vlc_inhibit_api type = sys->api;
    /* Receive reply from previous request, possibly hours later ;-) */
    if (sys->pending != NULL)
    {
        DBusMessage *reply;
        /* NOTE: Unfortunately, the pending reply cannot simply be cancelled.
         * Otherwise, the cookie would be lost and inhibition would remain on
         * (until complete disconnection from the bus). */
        dbus_pending_call_block(sys->pending);
        reply = dbus_pending_call_steal_reply(sys->pending);
        dbus_pending_call_unref(sys->pending);
        sys->pending = NULL;
        if (reply != NULL)
        {
            if (!dbus_message_get_args(reply, NULL,
                                       DBUS_TYPE_UINT32, &sys->cookie,
                                       DBUS_TYPE_INVALID))
                sys->cookie = 0;
            dbus_message_unref(reply);
        }
        msg_Dbg(ih, "got cookie %"PRIu32, (uint32_t)sys->cookie);
    }
    /* FIXME: This check is incorrect if flags change from one non-zero value
     * to another one. But the D-Bus API cannot currently inhibit suspend
     * independently from the screensaver. */
    if (!sys->cookie == !flags)
        return; /* nothing to do */
    /* Send request */
    const char *method = flags ? "Inhibit" : dbus_method_uninhibit[type];
    dbus_bool_t ret;
    DBusMessage *msg = dbus_message_new_method_call(dbus_service[type],
                                  dbus_path[type], dbus_service[type], method);
    if (unlikely(msg == NULL))
        return;
    if (flags)
    {
        const char *app = PACKAGE;
        const char *reason = _("Playing some media.");
        assert(sys->cookie == 0);
        switch (type)
        {
            case MATE:
            case GNOME:
            {
                dbus_uint32_t xid = 0; // FIXME ?
                dbus_uint32_t gflags = 0xC;
                ret = dbus_message_append_args(msg, DBUS_TYPE_STRING, &app,
                                                    DBUS_TYPE_UINT32, &xid,
                                                    DBUS_TYPE_STRING, &reason,
                                                    DBUS_TYPE_UINT32, &gflags,
                                                    DBUS_TYPE_INVALID);
                break;
            }
            default:
                ret = dbus_message_append_args(msg, DBUS_TYPE_STRING, &app,
                                                    DBUS_TYPE_STRING, &reason,
                                                    DBUS_TYPE_INVALID);
                break;
        }
        if (!ret
        || !dbus_connection_send_with_reply(sys->conn, msg, &sys->pending, -1))
            sys->pending = NULL;
    }
    else
    {
        assert(sys->cookie != 0);
        if (!dbus_message_append_args(msg, DBUS_TYPE_UINT32, &sys->cookie,
                                           DBUS_TYPE_INVALID)
         || !dbus_connection_send (sys->conn, msg, NULL))
            sys->cookie = 0;
    }
    dbus_connection_flush(sys->conn);
    dbus_message_unref(msg);
}

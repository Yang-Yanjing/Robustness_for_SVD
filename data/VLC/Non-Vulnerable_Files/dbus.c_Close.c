}
static void Close (vlc_object_t *obj)
{
    vlc_inhibit_t *ih = (vlc_inhibit_t *)obj;
    vlc_inhibit_sys_t *sys = ih->p_sys;
    if (sys->pending != NULL)
    {
        dbus_pending_call_cancel(sys->pending);
        dbus_pending_call_unref(sys->pending);
    }
    dbus_connection_close (sys->conn);
    dbus_connection_unref (sys->conn);
    free (sys);
}

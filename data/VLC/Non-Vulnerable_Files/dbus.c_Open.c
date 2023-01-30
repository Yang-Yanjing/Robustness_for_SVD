static void Close(vlc_object_t *obj);
static int Open (vlc_object_t *obj)
{
    vlc_inhibit_t *ih = (vlc_inhibit_t *)obj;
    vlc_inhibit_sys_t *sys = malloc (sizeof (*sys));
    if (unlikely(sys == NULL))
        return VLC_ENOMEM;
    DBusError err;
    dbus_error_init(&err);
    sys->conn = dbus_bus_get_private (DBUS_BUS_SESSION, &err);
    if (sys->conn == NULL)
    {
        msg_Err(ih, "cannot connect to session bus: %s", err.message);
        dbus_error_free(&err);
        free(sys);
        return VLC_EGENERIC;
    }
    sys->pending = NULL;
    sys->cookie = 0;
    ih->p_sys = sys;
    for (unsigned i = 0; i < MAX_API; i++)
    {
        if (dbus_bus_name_has_owner(sys->conn, dbus_service[i], NULL))
        {
            msg_Dbg(ih, "found service %s", dbus_service[i]);
            sys->api = i;
            ih->inhibit = Inhibit;
            return VLC_SUCCESS;
        }
        msg_Dbg(ih, "cannot find service %s", dbus_service[i]);
    }
    Close(obj);
    return VLC_EGENERIC;
}

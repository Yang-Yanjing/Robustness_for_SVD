}
static void Close (vlc_object_t *obj)
{
    services_discovery_t *sd = (services_discovery_t *)obj;
    services_discovery_sys_t *sys = sd->p_sys;
    vlc_pa_disconnect (obj, sys->context, sys->mainloop);
    tdestroy (sys->root, DestroySource);
    free (sys);
}

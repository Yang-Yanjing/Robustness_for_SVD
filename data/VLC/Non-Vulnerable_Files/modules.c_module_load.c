}
static int module_load (vlc_object_t *obj, module_t *m,
                        vlc_activate_t init, va_list args)
{
    int ret = VLC_SUCCESS;
    if (module_Map (obj, m))
        return VLC_EGENERIC;
    if (m->pf_activate != NULL)
    {
        va_list ap;
        va_copy (ap, args);
        ret = init (m->pf_activate, ap);
        va_end (ap);
    }
    return ret;
}

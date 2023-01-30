}
static void generic_stop(void *func, va_list ap)
{
    vlc_object_t *obj = va_arg(ap, vlc_object_t *);
    void (*deactivate)(vlc_object_t *) = func;
    deactivate(obj);
}

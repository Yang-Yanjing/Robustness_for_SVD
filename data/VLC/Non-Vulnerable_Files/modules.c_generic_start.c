}
static int generic_start(void *func, va_list ap)
{
    vlc_object_t *obj = va_arg(ap, vlc_object_t *);
    int (*activate)(vlc_object_t *) = func;
    return activate(obj);
}

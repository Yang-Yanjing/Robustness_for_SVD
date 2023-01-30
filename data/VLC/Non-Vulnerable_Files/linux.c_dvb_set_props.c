}
static int dvb_set_props (dvb_device_t *d, size_t n, ...)
{
    va_list ap;
    int ret;
    va_start (ap, n);
    ret = dvb_vset_props (d, n, ap);
    va_end (ap);
    return ret;
}

#endif
static int dvb_vset_props (dvb_device_t *d, size_t n, va_list ap)
{
    assert (n <= DTV_IOCTL_MAX_MSGS);
    struct dtv_property buf[n], *prop = buf;
    struct dtv_properties props = { .num = n, .props = buf };
    memset (buf, 0, sizeof (buf));
    while (n > 0)
    {
        prop->cmd = va_arg (ap, uint32_t);
        prop->u.data = va_arg (ap, uint32_t);
        msg_Dbg (d->obj, "setting property %2"PRIu32" to %"PRIu32,
                 prop->cmd, prop->u.data);
        prop++;
        n--;
    }
    if (ioctl (d->frontend, FE_SET_PROPERTY, &props) < 0)
    {
        msg_Err (d->obj, "cannot set frontend tuning parameters: %s",
                 vlc_strerror_c(errno));
        return -1;
    }
    return 0;
}

}
static int dvb_set_prop (dvb_device_t *d, uint32_t prop, uint32_t val)
{
    return dvb_set_props (d, 1, prop, val);
}

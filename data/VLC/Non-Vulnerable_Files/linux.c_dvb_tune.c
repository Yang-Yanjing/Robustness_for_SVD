}
int dvb_tune (dvb_device_t *d)
{
    return dvb_set_prop (d, DTV_TUNE, 0 /* dummy */);
}

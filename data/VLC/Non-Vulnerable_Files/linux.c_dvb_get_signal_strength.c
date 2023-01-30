}
float dvb_get_signal_strength (dvb_device_t *d)
{
    uint16_t strength;
    if (d->frontend == -1
     || ioctl (d->frontend, FE_READ_SIGNAL_STRENGTH, &strength) < 0)
        return 0.;
    return strength / 65535.;
}

}
float dvb_get_snr (dvb_device_t *d)
{
    uint16_t snr;
    if (d->frontend == -1 || ioctl (d->frontend, FE_READ_SNR, &snr) < 0)
        return 0.;
    return snr / 65535.;
}

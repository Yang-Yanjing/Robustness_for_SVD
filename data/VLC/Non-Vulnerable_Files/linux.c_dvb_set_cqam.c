}
int dvb_set_cqam (dvb_device_t *d, uint32_t freq, const char *modstr)
{
    unsigned mod = dvb_parse_modulation (modstr, QAM_AUTO);
    if (dvb_find_frontend (d, CQAM))
        return -1;
    return dvb_set_props (d, 4, DTV_CLEAR, 0,
                          DTV_DELIVERY_SYSTEM, SYS_DVBC_ANNEX_B,
                          DTV_FREQUENCY, freq, DTV_MODULATION, mod);
}

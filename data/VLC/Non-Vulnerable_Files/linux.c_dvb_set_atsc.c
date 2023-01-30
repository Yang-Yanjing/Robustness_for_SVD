/*** ATSC ***/
int dvb_set_atsc (dvb_device_t *d, uint32_t freq, const char *modstr)
{
    unsigned mod = dvb_parse_modulation (modstr, VSB_8);
    if (dvb_find_frontend (d, ATSC))
        return -1;
    return dvb_set_props (d, 4, DTV_CLEAR, 0, DTV_DELIVERY_SYSTEM, SYS_ATSC,
                          DTV_FREQUENCY, freq, DTV_MODULATION, mod);
}

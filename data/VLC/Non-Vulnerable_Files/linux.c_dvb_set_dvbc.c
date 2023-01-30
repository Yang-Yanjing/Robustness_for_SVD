/*** DVB-C ***/
int dvb_set_dvbc (dvb_device_t *d, uint32_t freq, const char *modstr,
                  uint32_t srate, uint32_t fec)
{
    unsigned mod = dvb_parse_modulation (modstr, QAM_AUTO);
    fec = dvb_parse_fec (fec);
    if (dvb_find_frontend (d, DVB_C))
        return -1;
    return dvb_set_props (d, 6, DTV_CLEAR, 0,
#if DVBv5(5)
                          DTV_DELIVERY_SYSTEM, SYS_DVBC_ANNEX_A,
#else
                          DTV_DELIVERY_SYSTEM, SYS_DVBC_ANNEX_AC,
#endif
                          DTV_FREQUENCY, freq, DTV_MODULATION, mod,
                          DTV_SYMBOL_RATE, srate, DTV_INNER_FEC, fec);
}

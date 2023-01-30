}
int dvb_set_dvbs2 (dvb_device_t *d, uint64_t freq_Hz, const char *modstr,
                   uint32_t srate, uint32_t fec, int pilot, int rolloff,
                   uint8_t sid)
{
    uint32_t freq = freq_Hz / 1000;
    unsigned mod = dvb_parse_modulation (modstr, QPSK);
    fec = dvb_parse_fec (fec);
    switch (pilot)
    {
        case 0:  pilot = PILOT_OFF;  break;
        case 1:  pilot = PILOT_ON;   break;
        default: pilot = PILOT_AUTO; break;
    }
    switch (rolloff)
    {
        case 20: rolloff = ROLLOFF_20;  break;
        case 25: rolloff = ROLLOFF_25;  break;
        case 35: rolloff = ROLLOFF_35;  break;
        default: rolloff = PILOT_AUTO; break;
    }
    if (dvb_find_frontend (d, DVB_S2))
        return -1;
#if DVBv5(8)
    return dvb_set_props (d, 9, DTV_CLEAR, 0, DTV_DELIVERY_SYSTEM, SYS_DVBS2,
                          DTV_FREQUENCY, freq, DTV_MODULATION, mod,
                          DTV_SYMBOL_RATE, srate, DTV_INNER_FEC, fec,
                          DTV_PILOT, pilot, DTV_ROLLOFF, rolloff,
                          DTV_STREAM_ID, (uint32_t)sid);
#else
# warning DVB-S2 needs Linux DVB version 5.8 or later.
    if (sid != 0)
    {
        msg_Err (d->obj, "DVB-S2 stream ID support not compiled-in");
        return -1;
    }
    return dvb_set_props (d, 8, DTV_CLEAR, 0,  DTV_DELIVERY_SYSTEM, SYS_DVBS2,
                          DTV_FREQUENCY, freq, DTV_MODULATION, mod,
                          DTV_SYMBOL_RATE, srate, DTV_INNER_FEC, fec,
                          DTV_PILOT, pilot, DTV_ROLLOFF, rolloff);
#endif
}

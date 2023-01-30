}
int dvb_set_dvbt2 (dvb_device_t *d, uint32_t freq, const char *modstr,
                   uint32_t fec, uint32_t bandwidth,
                   int transmit_mode, uint32_t guard, uint8_t plp)
{
#if DVBv5(3)
    uint32_t mod = dvb_parse_modulation (modstr, QAM_AUTO);
    fec = dvb_parse_fec (fec);
    bandwidth = dvb_parse_bandwidth (bandwidth);
    transmit_mode = dvb_parse_transmit_mode (transmit_mode);
    guard = dvb_parse_guard (guard);
    if (dvb_find_frontend (d, DVB_T2))
        return -1;
    return dvb_set_props (d, 9, DTV_CLEAR, 0, DTV_DELIVERY_SYSTEM, SYS_DVBT2,
                          DTV_FREQUENCY, freq, DTV_MODULATION, mod,
                          DTV_INNER_FEC, fec, DTV_BANDWIDTH_HZ, bandwidth,
                          DTV_TRANSMISSION_MODE, transmit_mode,
                          DTV_GUARD_INTERVAL, guard,
# if DVBv5(8)
                          DTV_STREAM_ID,
# else
                          DTV_DVBT2_PLP_ID,
# endif
                          (uint32_t)plp);
#else
# warning DVB-T2 needs Linux DVB version 5.3 or later.
    msg_Err (d->obj, "DVB-T2 support not compiled-in");
    (void) freq; (void) modstr; (void) fec; (void) bandwidth;
    (void) transmit_mode; (void) guard;
    return -1;
#endif
}

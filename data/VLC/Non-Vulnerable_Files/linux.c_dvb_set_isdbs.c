/*** ISDB-S ***/
int dvb_set_isdbs (dvb_device_t *d, uint64_t freq_Hz, uint16_t ts_id)
{
    uint32_t freq = freq_Hz / 1000;
    if (dvb_find_frontend (d, ISDB_S))
        return -1;
    return dvb_set_props (d, 4, DTV_CLEAR, 0, DTV_DELIVERY_SYSTEM, SYS_ISDBS,
                          DTV_FREQUENCY, freq,
#if DVBv5(8)
                          DTV_STREAM_ID,
#else
                          DTV_ISDBS_TS_ID,
#endif
                          (uint32_t)ts_id);
}

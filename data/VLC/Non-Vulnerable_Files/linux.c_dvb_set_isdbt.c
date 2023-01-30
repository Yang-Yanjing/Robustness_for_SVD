}
int dvb_set_isdbt (dvb_device_t *d, uint32_t freq, uint32_t bandwidth,
                   int transmit_mode, uint32_t guard,
                   const isdbt_layer_t layers[3])
{
    bandwidth = dvb_parse_bandwidth (bandwidth);
    transmit_mode = dvb_parse_transmit_mode (transmit_mode);
    guard = dvb_parse_guard (guard);
    if (dvb_find_frontend (d, ISDB_T))
        return -1;
    if (dvb_set_props (d, 5, DTV_CLEAR, 0, DTV_DELIVERY_SYSTEM, SYS_ISDBT,
                       DTV_FREQUENCY, freq, DTV_BANDWIDTH_HZ, bandwidth,
                       DTV_GUARD_INTERVAL, guard))
        return -1;
    for (unsigned i = 0; i < 3; i++)
        if (dvb_set_isdbt_layer (d, i, layers + i))
            return -1;
    return 0;
}

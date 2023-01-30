/*** ISDB-T ***/
static int dvb_set_isdbt_layer (dvb_device_t *d, unsigned num,
                                const isdbt_layer_t *l)
{
    uint32_t mod = dvb_parse_modulation (l->modulation, QAM_AUTO);
    uint32_t fec = dvb_parse_fec (l->code_rate);
    uint32_t count = l->segment_count;
    uint32_t ti = l->time_interleaving;
    num *= DTV_ISDBT_LAYERB_FEC - DTV_ISDBT_LAYERA_FEC;
    return dvb_set_props (d, 5, DTV_DELIVERY_SYSTEM, SYS_ISDBT,
                          DTV_ISDBT_LAYERA_FEC + num, fec,
                          DTV_ISDBT_LAYERA_MODULATION + num, mod,
                          DTV_ISDBT_LAYERA_SEGMENT_COUNT + num, count,
                          DTV_ISDBT_LAYERA_TIME_INTERLEAVING + num, ti);
}

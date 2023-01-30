/*** DVB-S ***/
static unsigned dvb_parse_polarization (char pol)
{
    static const dvb_int_map_t tab[5] = {
        { 0,   SEC_VOLTAGE_OFF },
        { 'H', SEC_VOLTAGE_18  },
        { 'L', SEC_VOLTAGE_18  },
        { 'R', SEC_VOLTAGE_13  },
        { 'V', SEC_VOLTAGE_13  },
    };
    return dvb_parse_int (pol, tab, 5, SEC_VOLTAGE_OFF);
}

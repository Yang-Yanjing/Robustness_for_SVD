}
static int dvb_parse_transmit_mode (int i)
{
    static const dvb_int_map_t tab[] = {
        { -1, TRANSMISSION_MODE_AUTO },
#if DVBv5(3)
        {  1, TRANSMISSION_MODE_1K   },
#endif
        {  2, TRANSMISSION_MODE_2K   },
        {  4, TRANSMISSION_MODE_4K   },
        {  8, TRANSMISSION_MODE_8K   },
#if DVBv5(3)
        { 16, TRANSMISSION_MODE_16K  },
        { 32, TRANSMISSION_MODE_32K  },
#endif
    };
    return dvb_parse_int (i, tab, sizeof (tab) / sizeof (*tab),
                          TRANSMISSION_MODE_AUTO);
}

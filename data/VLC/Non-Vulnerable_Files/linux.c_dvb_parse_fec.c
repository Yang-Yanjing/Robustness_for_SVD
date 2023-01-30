}
static int dvb_parse_fec (uint32_t fec)
{
    static const dvb_int_map_t rates[] =
    {
        { 0,             FEC_NONE },
        { VLC_FEC(1,2),  FEC_1_2  },
        // TODO: 1/3
        // TODO: 1/4
        { VLC_FEC(2,3),  FEC_2_3  },
        { VLC_FEC(3,4),  FEC_3_4  },
        { VLC_FEC(3,5),  FEC_3_5  },
        { VLC_FEC(4,5),  FEC_4_5  },
        { VLC_FEC(5,6),  FEC_5_6  },
        { VLC_FEC(6,7),  FEC_6_7  },
        { VLC_FEC(7,8),  FEC_7_8  },
        { VLC_FEC(8,9),  FEC_8_9  },
        { VLC_FEC(9,10), FEC_9_10 },
        { VLC_FEC_AUTO,  FEC_AUTO },
    };
    return dvb_parse_int (fec, rates, sizeof (rates) / sizeof (*rates),
                          FEC_AUTO);
}

}
static int dvb_parse_guard (uint32_t guard)
{
    static const dvb_int_map_t tab[] = {
        { VLC_GUARD(1,4),    GUARD_INTERVAL_1_4 },
        { VLC_GUARD(1,8),    GUARD_INTERVAL_1_8 },
        { VLC_GUARD(1,16),   GUARD_INTERVAL_1_16 },
        { VLC_GUARD(1,32),   GUARD_INTERVAL_1_32 },
#if DVBv5(3)
        { VLC_GUARD(1,128),  GUARD_INTERVAL_1_128 },
        { VLC_GUARD(19,128), GUARD_INTERVAL_19_128 },
        { VLC_GUARD(19,256), GUARD_INTERVAL_19_256 },
#endif
        { VLC_GUARD_AUTO,    GUARD_INTERVAL_AUTO },
    };
    return dvb_parse_int (guard, tab, sizeof (tab) / sizeof (*tab),
                          GUARD_INTERVAL_AUTO);
}

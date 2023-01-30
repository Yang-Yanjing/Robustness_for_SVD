 */
unsigned dvb_enum_systems (dvb_device_t *d)
{
    if (dvb_open_frontend (d))
        return 0;
#if DVBv5(5)
    struct dtv_property prop[2] = {
        { .cmd = DTV_API_VERSION },
        { .cmd = DTV_ENUM_DELSYS },
    };
    struct dtv_properties props = {
        .num = 2,
        .props = prop
    };
    if (ioctl (d->frontend, FE_GET_PROPERTY, &props) < 0)
    {
         msg_Err (d->obj, "cannot enumerate frontend systems: %s",
                  vlc_strerror_c(errno));
         goto legacy;
    }
    static const unsigned systab[] = {
        [SYS_UNDEFINED]    = 0,
        [SYS_DVBC_ANNEX_A] = DVB_C,
        [SYS_DVBC_ANNEX_B] = CQAM,
        [SYS_DVBT]         = DVB_T,
        //[SYS_DSS]
        [SYS_DVBS]         = DVB_S,
        [SYS_DVBS2]        = DVB_S2,
        //[SYS_DVBH]
        [SYS_ISDBT]        = ISDB_T,
        [SYS_ISDBS]        = ISDB_S,
        [SYS_ISDBC]        = ISDB_C, // no drivers exist (as of 3.3-rc6)
        [SYS_ATSC]         = ATSC,
        //[SYS_ATSCMH]
        //[SYS_DMBTH]
        //[SYS_CMMB]
        //[SYS_DAB]
        [SYS_DVBT2]        = DVB_T2,
        //[SYS_TURBO]
        [SYS_DVBC_ANNEX_C] = ISDB_C, // another name for ISDB-C?
    };
    unsigned systems = 0;
    msg_Dbg (d->obj, "probing frontend (kernel API v%u.%u, user API v%u.%u)",
             prop[0].u.data >> 8, prop[0].u.data & 0xFF,
             DVB_API_VERSION, DVB_API_VERSION_MINOR);
    for (size_t i = 0; i < prop[1].u.buffer.len; i++)
    {
        unsigned sys = prop[1].u.buffer.data[i];
        if (sys >= (sizeof (systab) / sizeof (systab[0])) || !systab[sys])
        {
            msg_Warn (d->obj, "unknown delivery system %u", sys);
            continue;
        }
        msg_Dbg (d->obj, " system %u", sys);
        systems |= systab[sys];
    }
    return systems;
legacy:
    props.num = 1;
#else
    struct dtv_property prop[1] = {
        { .cmd = DTV_API_VERSION },
    };
    struct dtv_properties props = {
        .num = 1,
        .props = prop
    };
    unsigned systems = 0;
#endif
    if (ioctl (d->frontend, FE_GET_PROPERTY, &props) < 0)
    {
        msg_Err (d->obj, "unsupported kernel DVB version 3 or older (%s)",
                 vlc_strerror_c(errno));
        return 0;
    }
    msg_Dbg (d->obj, "probing frontend (kernel API v%u.%u, user API v%u.%u)",
             prop[0].u.data >> 8, prop[0].u.data & 0xFF,
             DVB_API_VERSION, DVB_API_VERSION_MINOR);
#if !DVBv5(5)
    /* Some delivery systems cannot be detected without the DVB API v5.5.
     * To run correctly on recent kernels (Linux >= 3.3),
     * VLC needs to be compiled with up-to-date kernel headers. */
    if ((prop[0].u.data >> 8) > 5
     || ((prop[0].u.data >> 8) == 5 && (prop[0].u.data & 0xFF) >= 5))
        msg_Err (d->obj, "obsolete user API version running on a new kernel");
        msg_Info (d->obj, "please recompile "PACKAGE_NAME" "PACKAGE_VERSION);
#endif
    struct dvb_frontend_info info;
    if (ioctl (d->frontend, FE_GET_INFO, &info) < 0)
    {
        msg_Err (d->obj, "cannot get frontend info: %s",
                 vlc_strerror_c(errno));
        return 0;
    }
    msg_Dbg (d->obj, " name %s", info.name);
    msg_Dbg (d->obj, " type %u, capabilities 0x%08X", info.type, info.caps);
    msg_Dbg (d->obj, " frequencies %10"PRIu32" to %10"PRIu32,
             info.frequency_min, info.frequency_max);
    msg_Dbg (d->obj, " (%"PRIu32" tolerance, %"PRIu32" per step)",
             info.frequency_tolerance, info.frequency_stepsize);
    msg_Dbg (d->obj, " bauds rates %10"PRIu32" to %10"PRIu32,
             info.symbol_rate_min, info.symbol_rate_max);
    msg_Dbg (d->obj, " (%"PRIu32" tolerance)", info.symbol_rate_tolerance);
    /* DVB first generation and ATSC */
    switch (info.type)
    {
        case FE_QPSK: systems = DVB_S; break;
        case FE_QAM:  systems = DVB_C; break;
        case FE_OFDM: systems = DVB_T; break;
        case FE_ATSC: systems = ATSC | CQAM; break;
        default:
            msg_Err (d->obj, "unknown frontend type %u", info.type);
    }
    /* DVB 2nd generation */
    switch (info.type)
    {
        case FE_QPSK:
        case FE_QAM:
        case FE_OFDM:
            if (info.caps & FE_CAN_2G_MODULATION)
                systems |= systems << 1; /* DVB_foo -> DVB_foo|DVB_foo2 */
        default:
            break;
    }
    /* ISDB (only terrestrial before DVBv5.5)  */
    if (info.type == FE_OFDM)
        systems |= ISDB_T;
    return systems;
}

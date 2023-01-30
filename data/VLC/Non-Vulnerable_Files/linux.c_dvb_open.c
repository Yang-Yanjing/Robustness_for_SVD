 */
dvb_device_t *dvb_open (vlc_object_t *obj)
{
    dvb_device_t *d = malloc (sizeof (*d));
    if (unlikely(d == NULL))
        return NULL;
    d->obj = obj;
    uint8_t adapter = var_InheritInteger (obj, "dvb-adapter");
    d->device = var_InheritInteger (obj, "dvb-device");
    d->dir = dvb_open_adapter (adapter);
    if (d->dir == -1)
    {
        msg_Err (obj, "cannot access adapter %"PRIu8": %s", adapter,
                 vlc_strerror_c(errno));
        free (d);
        return NULL;
    }
    d->frontend = -1;
#ifdef HAVE_DVBPSI
    d->cam = NULL;
#endif
    d->budget = var_InheritBool (obj, "dvb-budget-mode");
#ifndef USE_DMX
    if (d->budget)
#endif
    {
       d->demux = dvb_open_node (d, "demux", O_RDONLY);
       if (d->demux == -1)
       {
           msg_Err (obj, "cannot access demultiplexer: %s",
                    vlc_strerror_c(errno));
           close (d->dir);
           free (d);
           return NULL;
       }
       if (ioctl (d->demux, DMX_SET_BUFFER_SIZE, 1 << 20) < 0)
           msg_Warn (obj, "cannot expand demultiplexing buffer: %s",
                     vlc_strerror_c(errno));
       /* We need to filter at least one PID. The tap for TS demultiplexing
        * cannot be configured otherwise. So add the PAT. */
        struct dmx_pes_filter_params param;
        param.pid = d->budget ? 0x2000 : 0x000;
        param.input = DMX_IN_FRONTEND;
        param.output = DMX_OUT_TSDEMUX_TAP;
        param.pes_type = DMX_PES_OTHER;
        param.flags = DMX_IMMEDIATE_START;
        if (ioctl (d->demux, DMX_SET_PES_FILTER, &param) < 0)
        {
            msg_Err (obj, "cannot setup TS demultiplexer: %s",
                     vlc_strerror_c(errno));
            goto error;
        }
#ifndef USE_DMX
    }
    else
    {
        for (size_t i = 0; i < MAX_PIDS; i++)
            d->pids[i].pid = d->pids[i].fd = -1;
        d->demux = dvb_open_node (d, "dvr", O_RDONLY);
        if (d->demux == -1)
        {
            msg_Err (obj, "cannot access DVR: %s", vlc_strerror_c(errno));
            close (d->dir);
            free (d);
            return NULL;
        }
#endif
    }
#ifdef HAVE_DVBPSI
    int ca = dvb_open_node (d, "ca", O_RDWR);
    if (ca != -1)
    {
        d->cam = en50221_Init (obj, ca);
        if (d->cam == NULL)
            close (ca);
    }
    else
        msg_Dbg (obj, "conditional access module not available: %s",
                 vlc_strerror_c(errno));
#endif
    return d;
error:
    dvb_close (d);
    return NULL;
}

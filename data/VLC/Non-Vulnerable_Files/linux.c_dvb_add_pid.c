}
int dvb_add_pid (dvb_device_t *d, uint16_t pid)
{
    if (d->budget)
        return 0;
#ifdef USE_DMX
    if (pid == 0 || ioctl (d->demux, DMX_ADD_PID, &pid) >= 0)
        return 0;
#else
    for (size_t i = 0; i < MAX_PIDS; i++)
    {
        if (d->pids[i].pid == pid)
            return 0;
        if (d->pids[i].fd != -1)
            continue;
        int fd = dvb_open_node (d, "demux", O_RDONLY);
        if (fd == -1)
            goto error;
       /* We need to filter at least one PID. The tap for TS demultiplexing
        * cannot be configured otherwise. So add the PAT. */
        struct dmx_pes_filter_params param;
        param.pid = pid;
        param.input = DMX_IN_FRONTEND;
        param.output = DMX_OUT_TS_TAP;
        param.pes_type = DMX_PES_OTHER;
        param.flags = DMX_IMMEDIATE_START;
        if (ioctl (fd, DMX_SET_PES_FILTER, &param) < 0)
        {
            close (fd);
            goto error;
        }
        d->pids[i].fd = fd;
        d->pids[i].pid = pid;
        return 0;
    }
    errno = EMFILE;
error:
#endif
    msg_Err (d->obj, "cannot add PID 0x%04"PRIu16": %s", pid,
             vlc_strerror_c(errno));
    return -1;
}

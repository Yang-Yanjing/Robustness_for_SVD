}
static int Seek (demux_t *demux, mtime_t pts)
{
    demux_sys_t *sys = demux->p_sys;
    /* Rewind if needed */
    if (pts < date_Get (&sys->pts) && SeekSet0 (demux))
        return VLC_EGENERIC;
    /* Fast forward */
    uint64_t pulse = sys->pulse;
    while (pts > date_Get (&sys->pts))
    {
        if (pulse == UINT64_MAX)
            return VLC_SUCCESS; /* premature end */
        if (ReadEvents (demux, &pulse, NULL))
            return VLC_EGENERIC;
    }
    sys->pulse = pulse;
    sys->tick = ((date_Get (&sys->pts) - VLC_TS_0) / TICK) * TICK + VLC_TS_0;
    return VLC_SUCCESS;
}

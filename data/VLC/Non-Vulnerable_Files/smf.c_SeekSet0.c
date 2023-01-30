}
static int SeekSet0 (demux_t *demux)
{
    stream_t *stream = demux->s;
    demux_sys_t *sys = demux->p_sys;
    /* Default SMF tempo is 120BPM, i.e. half a second per quarter note */
    date_Init (&sys->pts, sys->ppqn * 2, 1);
    date_Set (&sys->pts, VLC_TS_0);
    sys->pulse = 0;
    sys->tick = VLC_TS_0;
    for (unsigned i = 0; i < sys->trackc; i++)
    {
        mtrk_t *tr = sys->trackv + i;
        tr->offset = 0;
        tr->next = 0;
        /* Why 0xF6 (Tuning Calibration)?
         * Because it has zero bytes of data, so the parser will detect the
         * error if the first event uses running status. */
        tr->running_event = 0xF6;
        if (stream_Seek (stream, tr->start)
         || ReadDeltaTime (stream, tr))
        {
            msg_Err (demux, "fatal parsing error");
            return -1;
        }
    }
    return 0;
}

static void mpegts_free(MpegTSContext *ts)
{
    int i;
    clear_programs(ts);
    for (i = 0; i < NB_PID_MAX; i++)
        if (ts->pids[i])
            mpegts_close_filter(ts, ts->pids[i]);
}

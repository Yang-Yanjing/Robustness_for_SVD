static MpegTSFilter *mpegts_open_filter(MpegTSContext *ts, unsigned int pid,
                                        enum MpegTSFilterType type)
{
    MpegTSFilter *filter;
    av_log(ts->stream, AV_LOG_TRACE, "Filter: pid=0x%x\n", pid);
    if (pid >= NB_PID_MAX || ts->pids[pid])
        return NULL;
    filter = av_mallocz(sizeof(MpegTSFilter));
    if (!filter)
        return NULL;
    ts->pids[pid] = filter;
    filter->type    = type;
    filter->pid     = pid;
    filter->es_id   = -1;
    filter->last_cc = -1;
    filter->last_pcr= -1;
    return filter;
}

static void mpegts_close_filter(MpegTSContext *ts, MpegTSFilter *filter)
{
    int pid;
    pid = filter->pid;
    if (filter->type == MPEGTS_SECTION)
        av_freep(&filter->u.section_filter.section_buf);
    else if (filter->type == MPEGTS_PES) {
        PESContext *pes = filter->u.pes_filter.opaque;
        av_buffer_unref(&pes->buffer);
        

        if (!((PESContext *)filter->u.pes_filter.opaque)->st) {
            av_freep(&filter->u.pes_filter.opaque);
        }
    }
    av_free(filter);
    ts->pids[pid] = NULL;
}

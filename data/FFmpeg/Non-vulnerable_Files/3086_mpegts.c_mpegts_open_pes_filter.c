static MpegTSFilter *mpegts_open_pes_filter(MpegTSContext *ts, unsigned int pid,
                                            PESCallback *pes_cb,
                                            void *opaque)
{
    MpegTSFilter *filter;
    MpegTSPESFilter *pes;
    if (!(filter = mpegts_open_filter(ts, pid, MPEGTS_PES)))
        return NULL;
    pes = &filter->u.pes_filter;
    pes->pes_cb = pes_cb;
    pes->opaque = opaque;
    return filter;
}

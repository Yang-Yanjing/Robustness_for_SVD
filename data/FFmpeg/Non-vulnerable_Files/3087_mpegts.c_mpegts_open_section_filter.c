static MpegTSFilter *mpegts_open_section_filter(MpegTSContext *ts,
                                                unsigned int pid,
                                                SectionCallback *section_cb,
                                                void *opaque,
                                                int check_crc)
{
    MpegTSFilter *filter;
    MpegTSSectionFilter *sec;
    if (!(filter = mpegts_open_filter(ts, pid, MPEGTS_SECTION)))
        return NULL;
    sec = &filter->u.section_filter;
    sec->section_cb  = section_cb;
    sec->opaque      = opaque;
    sec->section_buf = av_malloc(MAX_SECTION_SIZE);
    sec->check_crc   = check_crc;
    sec->last_ver    = -1;
    if (!sec->section_buf) {
        av_free(filter);
        return NULL;
    }
    return filter;
}

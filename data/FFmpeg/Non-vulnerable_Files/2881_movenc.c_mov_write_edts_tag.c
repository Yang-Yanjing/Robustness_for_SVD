
static int mov_write_edts_tag(AVIOContext *pb, MOVMuxContext *mov,
                              MOVTrack *track)
{
    int64_t duration = av_rescale_rnd(track->track_duration, MOV_TIMESCALE,
                                      track->timescale, AV_ROUND_UP);
    int version = duration < INT32_MAX ? 0 : 1;
    int entry_size, entry_count, size;
    int64_t delay, start_ct = track->start_cts;
    int64_t start_dts = track->start_dts;
    if (track->entry) {
        if (start_dts != track->cluster[0].dts || start_ct != track->cluster[0].cts) {
            av_log(mov->fc, AV_LOG_DEBUG,
                   "EDTS using dts:%"PRId64" cts:%d instead of dts:%"PRId64" cts:%"PRId64" tid:%d\n",
                   track->cluster[0].dts, track->cluster[0].cts,
                   start_dts, start_ct, track->track_id);
            start_dts = track->cluster[0].dts;
            start_ct  = track->cluster[0].cts;
        }
    }
    delay = av_rescale_rnd(start_dts + start_ct, MOV_TIMESCALE,
                           track->timescale, AV_ROUND_DOWN);
    version |= delay < INT32_MAX ? 0 : 1;
    entry_size = (version == 1) ? 20 : 12;
    entry_count = 1 + (delay > 0);
    size = 24 + entry_count * entry_size;
    
    avio_wb32(pb, size);
    ffio_wfourcc(pb, "edts");
    avio_wb32(pb, size - 8);
    ffio_wfourcc(pb, "elst");
    avio_w8(pb, version);
    avio_wb24(pb, 0); 
    avio_wb32(pb, entry_count);
    if (delay > 0) { 
        




        if (version == 1) {
            avio_wb64(pb, delay);
            avio_wb64(pb, -1);
        } else {
            avio_wb32(pb, delay);
            avio_wb32(pb, -1);
        }
        avio_wb32(pb, 0x00010000);
    } else {
        



        av_assert0(av_rescale_rnd(start_dts, MOV_TIMESCALE, track->timescale, AV_ROUND_DOWN) <= 0);
        start_ct  = -FFMIN(start_dts, 0);
        


        duration += delay;
    }
    



    if (mov->flags & FF_MOV_FLAG_FRAGMENT)
        duration = 0;
    
    if (version == 1) {
        avio_wb64(pb, duration);
        avio_wb64(pb, start_ct);
    } else {
        avio_wb32(pb, duration);
        avio_wb32(pb, start_ct);
    }
    avio_wb32(pb, 0x00010000);
    return size;
}

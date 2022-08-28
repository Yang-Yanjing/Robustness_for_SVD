static int mov_write_mdhd_tag(AVIOContext *pb, MOVMuxContext *mov,
                              MOVTrack *track)
{
    int version = track->track_duration < INT32_MAX ? 0 : 1;
    if (track->mode == MODE_ISM)
        version = 1;
    (version == 1) ? avio_wb32(pb, 44) : avio_wb32(pb, 32); 
    ffio_wfourcc(pb, "mdhd");
    avio_w8(pb, version);
    avio_wb24(pb, 0); 
    if (version == 1) {
        avio_wb64(pb, track->time);
        avio_wb64(pb, track->time);
    } else {
        avio_wb32(pb, track->time); 
        avio_wb32(pb, track->time); 
    }
    avio_wb32(pb, track->timescale); 
    if (!track->entry && mov->mode == MODE_ISM)
        (version == 1) ? avio_wb64(pb, UINT64_C(0xffffffffffffffff)) : avio_wb32(pb, 0xffffffff);
    else if (!track->entry)
        (version == 1) ? avio_wb64(pb, 0) : avio_wb32(pb, 0);
    else
        (version == 1) ? avio_wb64(pb, track->track_duration) : avio_wb32(pb, track->track_duration); 
    avio_wb16(pb, track->language); 
    avio_wb16(pb, 0); 
    if (version != 0 && track->mode == MODE_MOV) {
        av_log(NULL, AV_LOG_ERROR,
               "FATAL error, file duration too long for timebase, this file will not be\n"
               "playable with quicktime. Choose a different timebase or a different\n"
               "container format\n");
    }
    return 32;
}

static int mov_write_dpxe_tag(AVIOContext *pb, MOVTrack *track)
{
    avio_wb32(pb, 12);
    ffio_wfourcc(pb, "DpxE");
    if (track->enc->extradata_size >= 12 &&
        !memcmp(&track->enc->extradata[4], "DpxE", 4)) {
        avio_wb32(pb, track->enc->extradata[11]);
    } else {
        avio_wb32(pb, 1);
    }
    return 0;
}

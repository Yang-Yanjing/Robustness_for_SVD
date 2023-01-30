static int mov_write_rtp_tag(AVIOContext *pb, MOVTrack *track)
{
    int64_t pos = avio_tell(pb);
    avio_wb32(pb, 0); 
    ffio_wfourcc(pb, "rtp ");
    avio_wb32(pb, 0); 
    avio_wb16(pb, 0); 
    avio_wb16(pb, 1); 
    avio_wb16(pb, 1); 
    avio_wb16(pb, 1); 
    avio_wb32(pb, track->max_packet_size); 
    avio_wb32(pb, 12); 
    ffio_wfourcc(pb, "tims");
    avio_wb32(pb, track->timescale);
    return update_size(pb, pos);
}

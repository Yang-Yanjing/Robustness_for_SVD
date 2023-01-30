static int mov_write_trex_tag(AVIOContext *pb, MOVTrack *track)
{
    avio_wb32(pb, 0x20); 
    ffio_wfourcc(pb, "trex");
    avio_wb32(pb, 0);   
    avio_wb32(pb, track->track_id); 
    avio_wb32(pb, 1);   
    avio_wb32(pb, 0);   
    avio_wb32(pb, 0);   
    avio_wb32(pb, 0);   
    return 0;
}

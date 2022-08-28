static int mov_write_gmhd_tag(AVIOContext *pb, MOVTrack *track)
{
    int64_t pos = avio_tell(pb);
    avio_wb32(pb, 0);      
    ffio_wfourcc(pb, "gmhd");
    avio_wb32(pb, 0x18);   
    ffio_wfourcc(pb, "gmin");
    avio_wb32(pb, 0);      
    avio_wb16(pb, 0x40);   
    avio_wb16(pb, 0x8000); 
    avio_wb16(pb, 0x8000); 
    avio_wb16(pb, 0x8000); 
    avio_wb16(pb, 0);      
    avio_wb16(pb, 0);      
    





    if (track->tag != MKTAG('c','6','0','8')) {
    avio_wb32(pb, 0x2C);   
    ffio_wfourcc(pb, "text");
    avio_wb16(pb, 0x01);
    avio_wb32(pb, 0x00);
    avio_wb32(pb, 0x00);
    avio_wb32(pb, 0x00);
    avio_wb32(pb, 0x01);
    avio_wb32(pb, 0x00);
    avio_wb32(pb, 0x00);
    avio_wb32(pb, 0x00);
    avio_wb32(pb, 0x00004000);
    avio_wb16(pb, 0x0000);
    }
    if (track->enc->codec_tag == MKTAG('t','m','c','d')) {
        int64_t tmcd_pos = avio_tell(pb);
        avio_wb32(pb, 0); 
        ffio_wfourcc(pb, "tmcd");
        mov_write_tcmi_tag(pb, track);
        update_size(pb, tmcd_pos);
    }
    return update_size(pb, pos);
}

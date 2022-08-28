static int mov_write_hmhd_tag(AVIOContext *pb)
{
    

    avio_wb32(pb, 28); 
    ffio_wfourcc(pb, "hmhd");
    avio_wb32(pb, 0); 
    avio_wb16(pb, 0); 
    avio_wb16(pb, 0); 
    avio_wb32(pb, 0); 
    avio_wb32(pb, 0); 
    avio_wb32(pb, 0); 
    return 28;
}

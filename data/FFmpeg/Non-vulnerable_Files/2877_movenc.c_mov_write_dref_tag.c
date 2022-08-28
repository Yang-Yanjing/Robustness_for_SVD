static int mov_write_dref_tag(AVIOContext *pb)
{
    avio_wb32(pb, 28); 
    ffio_wfourcc(pb, "dref");
    avio_wb32(pb, 0); 
    avio_wb32(pb, 1); 
    avio_wb32(pb, 0xc); 
    
    ffio_wfourcc(pb, "url ");
    avio_wb32(pb, 1); 
    return 28;
}

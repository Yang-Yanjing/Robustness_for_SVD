static int mov_write_smhd_tag(AVIOContext *pb)
{
    avio_wb32(pb, 16); 
    ffio_wfourcc(pb, "smhd");
    avio_wb32(pb, 0); 
    avio_wb16(pb, 0); 
    avio_wb16(pb, 0); 
    return 16;
}

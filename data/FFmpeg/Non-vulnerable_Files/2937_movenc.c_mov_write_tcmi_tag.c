static int mov_write_tcmi_tag(AVIOContext *pb, MOVTrack *track)
{
    int64_t pos = avio_tell(pb);
    const char *font = "Lucida Grande";
    avio_wb32(pb, 0);                   
    ffio_wfourcc(pb, "tcmi");           
    avio_wb32(pb, 0);                   
    avio_wb16(pb, 0);                   
    avio_wb16(pb, 0);                   
    avio_wb16(pb, 12);                  
    avio_wb16(pb, 0);                   
    avio_wb16(pb, 0x0000);              
    avio_wb16(pb, 0x0000);              
    avio_wb16(pb, 0x0000);              
    avio_wb16(pb, 0xffff);              
    avio_wb16(pb, 0xffff);              
    avio_wb16(pb, 0xffff);              
    avio_w8(pb, strlen(font));          
    avio_write(pb, font, strlen(font)); 
    return update_size(pb, pos);
}

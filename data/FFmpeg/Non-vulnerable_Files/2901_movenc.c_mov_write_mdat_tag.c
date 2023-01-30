static int mov_write_mdat_tag(AVIOContext *pb, MOVMuxContext *mov)
{
    avio_wb32(pb, 8);    
    ffio_wfourcc(pb, mov->mode == MODE_MOV ? "wide" : "free");
    mov->mdat_pos = avio_tell(pb);
    avio_wb32(pb, 0); 
    ffio_wfourcc(pb, "mdat");
    return 0;
}

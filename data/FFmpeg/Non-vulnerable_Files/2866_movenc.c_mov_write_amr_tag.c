static int mov_write_amr_tag(AVIOContext *pb, MOVTrack *track)
{
    avio_wb32(pb, 0x11); 
    if (track->mode == MODE_MOV) ffio_wfourcc(pb, "samr");
    else                         ffio_wfourcc(pb, "damr");
    ffio_wfourcc(pb, "FFMP");
    avio_w8(pb, 0); 
    avio_wb16(pb, 0x81FF); 
    avio_w8(pb, 0x00); 
    avio_w8(pb, 0x01); 
    return 0x11;
}

static int mov_write_meta_tag(AVIOContext *pb, MOVMuxContext *mov,
                              AVFormatContext *s)
{
    int size = 0;
    int64_t pos = avio_tell(pb);
    avio_wb32(pb, 0); 
    ffio_wfourcc(pb, "meta");
    avio_wb32(pb, 0);
    mov_write_itunes_hdlr_tag(pb, mov, s);
    mov_write_ilst_tag(pb, mov, s);
    size = update_size(pb, pos);
    return size;
}

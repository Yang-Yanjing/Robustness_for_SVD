static int mov_write_mdia_tag(AVIOContext *pb, MOVMuxContext *mov,
                              MOVTrack *track)
{
    int64_t pos = avio_tell(pb);
    int ret;
    avio_wb32(pb, 0); 
    ffio_wfourcc(pb, "mdia");
    mov_write_mdhd_tag(pb, mov, track);
    mov_write_hdlr_tag(pb, track);
    if ((ret = mov_write_minf_tag(pb, mov, track)) < 0)
        return ret;
    return update_size(pb, pos);
}

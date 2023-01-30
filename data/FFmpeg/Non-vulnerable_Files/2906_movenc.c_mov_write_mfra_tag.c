static int mov_write_mfra_tag(AVIOContext *pb, MOVMuxContext *mov)
{
    int64_t pos = avio_tell(pb);
    int i;
    avio_wb32(pb, 0); 
    ffio_wfourcc(pb, "mfra");
    

    if (mov->flags & FF_MOV_FLAG_ISML)
        return update_size(pb, pos);
    for (i = 0; i < mov->nb_streams; i++) {
        MOVTrack *track = &mov->tracks[i];
        if (track->nb_frag_info)
            mov_write_tfra_tag(pb, track);
    }
    avio_wb32(pb, 16);
    ffio_wfourcc(pb, "mfro");
    avio_wb32(pb, 0); 
    avio_wb32(pb, avio_tell(pb) + 4 - pos);
    return update_size(pb, pos);
}

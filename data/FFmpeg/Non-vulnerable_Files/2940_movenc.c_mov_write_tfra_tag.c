static int mov_write_tfra_tag(AVIOContext *pb, MOVTrack *track)
{
    int64_t pos = avio_tell(pb);
    int i;
    avio_wb32(pb, 0); 
    ffio_wfourcc(pb, "tfra");
    avio_w8(pb, 1); 
    avio_wb24(pb, 0);
    avio_wb32(pb, track->track_id);
    avio_wb32(pb, 0); 
    avio_wb32(pb, track->nb_frag_info);
    for (i = 0; i < track->nb_frag_info; i++) {
        avio_wb64(pb, track->frag_info[i].time);
        avio_wb64(pb, track->frag_info[i].offset + track->data_offset);
        avio_w8(pb, 1); 
        avio_w8(pb, 1); 
        avio_w8(pb, 1); 
    }
    return update_size(pb, pos);
}

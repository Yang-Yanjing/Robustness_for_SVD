static int mov_write_subtitle_tag(AVIOContext *pb, MOVTrack *track)
{
    int64_t pos = avio_tell(pb);
    avio_wb32(pb, 0);    
    avio_wl32(pb, track->tag); 
    avio_wb32(pb, 0);    
    avio_wb16(pb, 0);    
    avio_wb16(pb, 1);    
    if (track->enc->codec_id == AV_CODEC_ID_DVD_SUBTITLE)
        mov_write_esds_tag(pb, track);
    else if (track->enc->extradata_size)
        avio_write(pb, track->enc->extradata, track->enc->extradata_size);
    return update_size(pb, pos);
}

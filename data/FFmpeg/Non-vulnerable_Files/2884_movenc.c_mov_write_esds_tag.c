static int mov_write_esds_tag(AVIOContext *pb, MOVTrack *track) 
{
    int64_t pos = avio_tell(pb);
    int decoder_specific_info_len = track->vos_len ? 5 + track->vos_len : 0;
    unsigned avg_bitrate;
    avio_wb32(pb, 0); 
    ffio_wfourcc(pb, "esds");
    avio_wb32(pb, 0); 
    
    put_descr(pb, 0x03, 3 + 5+13 + decoder_specific_info_len + 5+1);
    avio_wb16(pb, track->track_id);
    avio_w8(pb, 0x00); 
    
    put_descr(pb, 0x04, 13 + decoder_specific_info_len);
    
    if ((track->enc->codec_id == AV_CODEC_ID_MP2 ||
         track->enc->codec_id == AV_CODEC_ID_MP3) &&
        track->enc->sample_rate > 24000)
        avio_w8(pb, 0x6B); 
    else
        avio_w8(pb, ff_codec_get_tag(ff_mp4_obj_type, track->enc->codec_id));
    
    
    if (track->enc->codec_id == AV_CODEC_ID_DVD_SUBTITLE)
        avio_w8(pb, (0x38 << 2) | 1); 
    else if (track->enc->codec_type == AVMEDIA_TYPE_AUDIO)
        avio_w8(pb, 0x15); 
    else
        avio_w8(pb, 0x11); 
    avio_wb24(pb, track->enc->rc_buffer_size >> 3); 
    avg_bitrate = compute_avg_bitrate(track);
    
    avio_wb32(pb, FFMAX3(track->enc->bit_rate, track->enc->rc_max_rate, avg_bitrate));
    avio_wb32(pb, avg_bitrate);
    if (track->vos_len) {
        
        put_descr(pb, 0x05, track->vos_len);
        avio_write(pb, track->vos_data, track->vos_len);
    }
    
    put_descr(pb, 0x06, 1);
    avio_w8(pb, 0x02);
    return update_size(pb, pos);
}

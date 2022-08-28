static int mov_write_chan_tag(AVIOContext *pb, MOVTrack *track)
{
    uint32_t layout_tag, bitmap;
    int64_t pos = avio_tell(pb);
    layout_tag = ff_mov_get_channel_layout_tag(track->enc->codec_id,
                                               track->enc->channel_layout,
                                               &bitmap);
    if (!layout_tag) {
        av_log(track->enc, AV_LOG_WARNING, "not writing 'chan' tag due to "
               "lack of channel information\n");
        return 0;
    }
    if (track->multichannel_as_mono)
        return 0;
    avio_wb32(pb, 0);           
    ffio_wfourcc(pb, "chan");   
    avio_w8(pb, 0);             
    avio_wb24(pb, 0);           
    avio_wb32(pb, layout_tag);  
    avio_wb32(pb, bitmap);      
    avio_wb32(pb, 0);           
    return update_size(pb, pos);
}

static void mxf_write_generic_sound_common(AVFormatContext *s, AVStream *st, const UID key, unsigned size)
{
    AVIOContext *pb = s->pb;
    MXFContext *mxf = s->priv_data;
    int show_warnings = !mxf->footer_partition_offset;
    int duration_size = 0;
    if (s->oformat == &ff_mxf_opatom_muxer)
        duration_size = 12;
    mxf_write_generic_desc(s, st, key, size+duration_size+5+12+8+8);
    if (duration_size > 0){
        mxf_write_local_tag(pb, 8, 0x3002);
        avio_wb64(pb, mxf->body_offset / mxf->edit_unit_byte_count);
    }
    
    mxf_write_local_tag(pb, 1, 0x3D02);
    avio_w8(pb, 1);
    
    mxf_write_local_tag(pb, 8, 0x3D03);
    avio_wb32(pb, st->codec->sample_rate);
    avio_wb32(pb, 1);
    mxf_write_local_tag(pb, 4, 0x3D07);
    if (mxf->channel_count == -1) {
        if (show_warnings && (s->oformat == &ff_mxf_d10_muxer) && (st->codec->channels != 4) && (st->codec->channels != 8))
            av_log(s, AV_LOG_WARNING, "the number of audio channels shall be 4 or 8 : the output will not comply to MXF D-10 specs, use -d10_channelcount to fix this\n");
        avio_wb32(pb, st->codec->channels);
    } else if (s->oformat == &ff_mxf_d10_muxer) {
        if (show_warnings && (mxf->channel_count < st->codec->channels))
            av_log(s, AV_LOG_WARNING, "d10_channelcount < actual number of audio channels : some channels will be discarded\n");
        if (show_warnings && (mxf->channel_count != 4) && (mxf->channel_count != 8))
            av_log(s, AV_LOG_WARNING, "d10_channelcount shall be set to 4 or 8 : the output will not comply to MXF D-10 specs\n");
        avio_wb32(pb, mxf->channel_count);
    } else {
        if (show_warnings && mxf->channel_count != -1 && s->oformat != &ff_mxf_opatom_muxer)
            av_log(s, AV_LOG_ERROR, "-d10_channelcount requires MXF D-10 and will be ignored\n");
        avio_wb32(pb, st->codec->channels);
    }
    mxf_write_local_tag(pb, 4, 0x3D01);
    avio_wb32(pb, av_get_bits_per_sample(st->codec->codec_id));
}

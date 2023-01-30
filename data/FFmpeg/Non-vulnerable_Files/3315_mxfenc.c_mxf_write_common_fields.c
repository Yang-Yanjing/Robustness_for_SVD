static void mxf_write_common_fields(AVFormatContext *s, AVStream *st)
{
    MXFContext *mxf = s->priv_data;
    AVIOContext *pb = s->pb;
    
    mxf_write_local_tag(pb, 16, 0x0201);
    if (st == mxf->timecode_track)
        avio_write(pb, smpte_12m_timecode_track_data_ul, 16);
    else {
        const MXFCodecUL *data_def_ul = mxf_get_data_definition_ul(st->codec->codec_type);
        avio_write(pb, data_def_ul->uid, 16);
    }
    
    mxf_write_local_tag(pb, 8, 0x0202);
    if (st != mxf->timecode_track && s->oformat == &ff_mxf_opatom_muxer && st->codec->codec_type == AVMEDIA_TYPE_AUDIO){
        avio_wb64(pb, mxf->body_offset / mxf->edit_unit_byte_count);
    } else {
        avio_wb64(pb, mxf->duration);
    }
}

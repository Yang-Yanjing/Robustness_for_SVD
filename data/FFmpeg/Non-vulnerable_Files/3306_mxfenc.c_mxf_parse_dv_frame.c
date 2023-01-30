static int mxf_parse_dv_frame(AVFormatContext *s, AVStream *st, AVPacket *pkt)
{
    MXFContext *mxf = s->priv_data;
    MXFStreamContext *sc = st->priv_data;
    uint8_t *vs_pack, *vsc_pack;
    int i, ul_index, frame_size, stype, pal;
    if (mxf->header_written)
        return 1;
    
    if (pkt->size < 120000)
        return -1;
    vs_pack  = pkt->data + 80*5 + 48;
    vsc_pack = pkt->data + 80*5 + 53;
    stype    = vs_pack[3] & 0x1f;
    pal      = (vs_pack[3] >> 5) & 0x1;
    if ((vs_pack[2] & 0x07) == 0x02)
        sc->aspect_ratio = (AVRational){ 16, 9 };
    else
        sc->aspect_ratio = (AVRational){ 4, 3 };
    sc->interlaced = (vsc_pack[3] >> 4) & 0x01;
    
    
    
    switch (stype) {
    case 0x18: 
        ul_index = 6 + pal;
        frame_size = pal ? 288000 : 240000;
        if (sc->interlaced) {
            av_log(s, AV_LOG_ERROR, "source marked as interlaced but codec profile is progressive\n");
            sc->interlaced = 0;
        }
        break;
    case 0x14: 
        ul_index = 4 + pal;
        frame_size = pal ? 576000 : 480000;
        break;
    case 0x04: 
        ul_index = 2 + pal;
        frame_size = pal ? 288000 : 240000;
        break;
    default: 
        ul_index = 0 + pal;
        frame_size = pal ? 144000 : 120000;
    }
    sc->index = ul_index + 16;
    sc->codec_ul =  &mxf_essence_container_uls[sc->index].codec_ul;
    if(s->oformat == &ff_mxf_opatom_muxer) {
        mxf->edit_unit_byte_count = frame_size;
        return 1;
    }
    mxf->edit_unit_byte_count = KAG_SIZE;
    for (i = 0; i < s->nb_streams; i++) {
        AVStream *st = s->streams[i];
        MXFStreamContext *sc = st->priv_data;
        if (st->codec->codec_type == AVMEDIA_TYPE_AUDIO) {
            mxf->edit_unit_byte_count += 16 + 4 + sc->aic.samples[0]*sc->aic.sample_size;
            mxf->edit_unit_byte_count += klv_fill_size(mxf->edit_unit_byte_count);
        } else if (st->codec->codec_type == AVMEDIA_TYPE_VIDEO) {
            mxf->edit_unit_byte_count += 16 + 4 + frame_size;
            mxf->edit_unit_byte_count += klv_fill_size(mxf->edit_unit_byte_count);
        }
    }
    return 1;
}

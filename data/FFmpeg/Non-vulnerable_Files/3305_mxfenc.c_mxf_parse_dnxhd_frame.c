static int mxf_parse_dnxhd_frame(AVFormatContext *s, AVStream *st,
AVPacket *pkt)
{
    MXFContext *mxf = s->priv_data;
    MXFStreamContext *sc = st->priv_data;
    int i, cid;
    uint8_t* header_cid;
    int frame_size = 0;
    if (mxf->header_written)
        return 1;
    if (pkt->size < 43)
        return -1;
    header_cid = pkt->data + 0x28;
    cid = header_cid[0] << 24 | header_cid[1] << 16 | header_cid[2] << 8 | header_cid[3];
    if ((frame_size = avpriv_dnxhd_get_frame_size(cid)) < 0)
        return -1;
    if ((sc->interlaced = avpriv_dnxhd_get_interlaced(cid)) < 0)
        return AVERROR_INVALIDDATA;
    switch (cid) {
    case 1235:
        sc->index = 24;
        sc->component_depth = 10;
        break;
    case 1237:
        sc->index = 25;
        break;
    case 1238:
        sc->index = 26;
        break;
    case 1241:
        sc->index = 27;
        sc->component_depth = 10;
        break;
    case 1242:
        sc->index = 28;
        break;
    case 1243:
        sc->index = 29;
        break;
    case 1250:
        sc->index = 30;
        sc->component_depth = 10;
        break;
    case 1251:
        sc->index = 31;
        break;
    case 1252:
        sc->index = 32;
        break;
    case 1253:
        sc->index = 33;
        break;
    default:
        return -1;
    }
    sc->codec_ul = &mxf_essence_container_uls[sc->index].codec_ul;
    sc->aspect_ratio = (AVRational){ 16, 9 };
    if(s->oformat == &ff_mxf_opatom_muxer){
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

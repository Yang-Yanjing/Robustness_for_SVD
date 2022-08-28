static int mxf_parse_mpeg2_frame(AVFormatContext *s, AVStream *st,
                                 AVPacket *pkt, MXFIndexEntry *e)
{
    MXFStreamContext *sc = st->priv_data;
    uint32_t c = -1;
    int i;
    for(i = 0; i < pkt->size - 4; i++) {
        c = (c<<8) + pkt->data[i];
        if (c == 0x1b5) {
            if ((pkt->data[i+1] & 0xf0) == 0x10) { 
                st->codec->profile = pkt->data[i+1] & 0x07;
                st->codec->level   = pkt->data[i+2] >> 4;
            } else if (i + 5 < pkt->size && (pkt->data[i+1] & 0xf0) == 0x80) { 
                sc->interlaced = !(pkt->data[i+5] & 0x80); 
                if (sc->interlaced)
                    sc->field_dominance = 1 + !(pkt->data[i+4] & 0x80); 
                break;
            }
        } else if (c == 0x1b8) { 
            if (pkt->data[i+4]>>6 & 0x01) { 
                sc->closed_gop = 1;
                if (e->flags & 0x40) 
                    e->flags |= 0x80; 
            }
        } else if (c == 0x1b3) { 
            e->flags |= 0x40;
            switch ((pkt->data[i+4]>>4) & 0xf) {
            case 2:  sc->aspect_ratio = (AVRational){  4,  3}; break;
            case 3:  sc->aspect_ratio = (AVRational){ 16,  9}; break;
            case 4:  sc->aspect_ratio = (AVRational){221,100}; break;
            default:
                av_reduce(&sc->aspect_ratio.num, &sc->aspect_ratio.den,
                          st->codec->width, st->codec->height, 1024*1024);
            }
        } else if (c == 0x100) { 
            int pict_type = (pkt->data[i+2]>>3) & 0x07;
            e->temporal_ref = (pkt->data[i+1]<<2) | (pkt->data[i+2]>>6);
            if (pict_type == 2) { 
                e->flags |= 0x22;
                sc->closed_gop = 0; 
            } else if (pict_type == 3) { 
                if (sc->closed_gop)
                    e->flags |= 0x13; 
                else
                    e->flags |= 0x33;
                sc->temporal_reordering = -1;
            } else if (!pict_type) {
                av_log(s, AV_LOG_ERROR, "error parsing mpeg2 frame\n");
                return 0;
            }
        }
    }
    if (s->oformat != &ff_mxf_d10_muxer)
        sc->codec_ul = mxf_get_mpeg2_codec_ul(st->codec);
    return !!sc->codec_ul;
}

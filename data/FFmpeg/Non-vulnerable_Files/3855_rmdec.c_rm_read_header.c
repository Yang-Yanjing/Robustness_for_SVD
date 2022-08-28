static int rm_read_header(AVFormatContext *s)
{
    RMDemuxContext *rm = s->priv_data;
    AVStream *st;
    AVIOContext *pb = s->pb;
    unsigned int tag;
    int tag_size;
    unsigned int start_time, duration;
    unsigned int data_off = 0, indx_off = 0;
    char buf[128], mime[128];
    int flags = 0;
    int ret = -1;
    unsigned size, v;
    int64_t codec_pos;
    tag = avio_rl32(pb);
    if (tag == MKTAG('.', 'r', 'a', 0xfd)) {
        
        return rm_read_header_old(s);
    } else if (tag != MKTAG('.', 'R', 'M', 'F')) {
        return AVERROR(EIO);
    }
    tag_size = avio_rb32(pb);
    avio_skip(pb, tag_size - 8);
    for(;;) {
        if (avio_feof(pb))
            goto fail;
        tag = avio_rl32(pb);
        tag_size = avio_rb32(pb);
        avio_rb16(pb);
        av_log(s, AV_LOG_TRACE, "tag=%c%c%c%c (%08x) size=%d\n",
                (tag      ) & 0xff,
                (tag >>  8) & 0xff,
                (tag >> 16) & 0xff,
                (tag >> 24) & 0xff,
                tag,
                tag_size);
        if (tag_size < 10 && tag != MKTAG('D', 'A', 'T', 'A'))
            goto fail;
        switch(tag) {
        case MKTAG('P', 'R', 'O', 'P'):
            
            avio_rb32(pb); 
            avio_rb32(pb); 
            avio_rb32(pb); 
            avio_rb32(pb); 
            avio_rb32(pb); 
            duration = avio_rb32(pb); 
            s->duration = av_rescale(duration, AV_TIME_BASE, 1000);
            avio_rb32(pb); 
            indx_off = avio_rb32(pb); 
            data_off = avio_rb32(pb); 
            avio_rb16(pb); 
            flags = avio_rb16(pb); 
            break;
        case MKTAG('C', 'O', 'N', 'T'):
            rm_read_metadata(s, pb, 1);
            break;
        case MKTAG('M', 'D', 'P', 'R'):
            st = avformat_new_stream(s, NULL);
            if (!st) {
                ret = AVERROR(ENOMEM);
                goto fail;
            }
            st->id = avio_rb16(pb);
            avio_rb32(pb); 
            st->codec->bit_rate = avio_rb32(pb); 
            avio_rb32(pb); 
            avio_rb32(pb); 
            start_time = avio_rb32(pb); 
            avio_rb32(pb); 
            duration = avio_rb32(pb); 
            st->start_time = start_time;
            st->duration = duration;
            if(duration>0)
                s->duration = AV_NOPTS_VALUE;
            get_str8(pb, buf, sizeof(buf)); 
            get_str8(pb, mime, sizeof(mime)); 
            st->codec->codec_type = AVMEDIA_TYPE_DATA;
            st->priv_data = ff_rm_alloc_rmstream();
            if (!st->priv_data)
                return AVERROR(ENOMEM);
            size = avio_rb32(pb);
            codec_pos = avio_tell(pb);
            ffio_ensure_seekback(pb, 4);
            v = avio_rb32(pb);
            if (v == MKBETAG('M', 'L', 'T', 'I')) {
                int number_of_streams = avio_rb16(pb);
                int number_of_mdpr;
                int i;
                unsigned size2;
                for (i = 0; i<number_of_streams; i++)
                    avio_rb16(pb);
                number_of_mdpr = avio_rb16(pb);
                if (number_of_mdpr != 1) {
                    avpriv_request_sample(s, "MLTI with multiple (%d) MDPR", number_of_mdpr);
                }
                for (i = 0; i < number_of_mdpr; i++) {
                    AVStream *st2;
                    if (i > 0) {
                        st2 = avformat_new_stream(s, NULL);
                        if (!st2) {
                            ret = AVERROR(ENOMEM);
                            goto fail;
                        }
                        st2->id = st->id + (i<<16);
                        st2->codec->bit_rate = st->codec->bit_rate;
                        st2->start_time = st->start_time;
                        st2->duration   = st->duration;
                        st2->codec->codec_type = AVMEDIA_TYPE_DATA;
                        st2->priv_data = ff_rm_alloc_rmstream();
                        if (!st2->priv_data)
                            return AVERROR(ENOMEM);
                    } else
                        st2 = st;
                    size2 = avio_rb32(pb);
                    if (ff_rm_read_mdpr_codecdata(s, s->pb, st2, st2->priv_data,
                                                  size2, mime) < 0)
                        goto fail;
                }
                avio_seek(pb, codec_pos + size, SEEK_SET);
            } else {
                avio_skip(pb, -4);
                if (ff_rm_read_mdpr_codecdata(s, s->pb, st, st->priv_data,
                                              size, mime) < 0)
                    goto fail;
            }
            break;
        case MKTAG('D', 'A', 'T', 'A'):
            goto header_end;
        default:
            
            avio_skip(pb, tag_size - 10);
            break;
        }
    }
 header_end:
    rm->nb_packets = avio_rb32(pb); 
    if (!rm->nb_packets && (flags & 4))
        rm->nb_packets = 3600 * 25;
    avio_rb32(pb); 
    if (!data_off)
        data_off = avio_tell(pb) - 18;
    if (indx_off && pb->seekable && !(s->flags & AVFMT_FLAG_IGNIDX) &&
        avio_seek(pb, indx_off, SEEK_SET) >= 0) {
        rm_read_index(s);
        avio_seek(pb, data_off + 18, SEEK_SET);
    }
    return 0;
fail:
    rm_read_close(s);
    return ret;
}

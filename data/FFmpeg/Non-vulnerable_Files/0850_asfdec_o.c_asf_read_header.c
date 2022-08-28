static int asf_read_header(AVFormatContext *s)
{
    ASFContext *asf         = s->priv_data;
    AVIOContext *pb         = s->pb;
    const GUIDParseTable *g = NULL;
    ff_asf_guid guid;
    int i, ret;
    uint64_t size;
    asf->preroll         = 0;
    asf->is_simple_index = 0;
    ff_get_guid(pb, &guid);
    if (ff_guidcmp(&guid, &ff_asf_header))
        return AVERROR_INVALIDDATA;
    avio_skip(pb, 8); 
    avio_skip(pb, 6); 
    asf->data_reached = 0;
    



    while (1) {
        
        if (avio_tell(pb) == asf->offset)
            break;
        asf->offset = avio_tell(pb);
        if ((ret = ff_get_guid(pb, &guid)) < 0) {
            if (ret == AVERROR_EOF && asf->data_reached)
                break;
            else
                goto failed;
        }
        g = find_guid(guid);
        if (g) {
            asf->unknown_offset = asf->offset;
            asf->is_header = 1;
            if ((ret = g->read_object(s, g)) < 0)
                goto failed;
        } else {
            size = avio_rl64(pb);
            align_position(pb, asf->offset, size);
        }
        if (asf->data_reached && (!pb->seekable || (asf->b_flags & ASF_FLAG_BROADCAST)))
            break;
    }
    if (!asf->data_reached) {
        av_log(s, AV_LOG_ERROR, "Data Object was not found.\n");
        ret = AVERROR_INVALIDDATA;
        goto failed;
    }
    if (pb->seekable)
        avio_seek(pb, asf->first_packet_offset, SEEK_SET);
    for (i = 0; i < asf->nb_streams; i++) {
        const char *rfc1766 = asf->asf_sd[asf->asf_st[i]->lang_idx].langs;
        AVStream *st        = s->streams[asf->asf_st[i]->index];
        set_language(s, rfc1766, &st->metadata);
    }
    for (i = 0; i < ASF_MAX_STREAMS; i++) {
        AVStream *st = NULL;
        st = find_stream(s, i);
        if (st) {
            av_dict_copy(&st->metadata, asf->asf_sd[i].asf_met, AV_DICT_IGNORE_SUFFIX);
            if (asf->asf_sd[i].aspect_ratio.num > 0 && asf->asf_sd[i].aspect_ratio.den > 0) {
                st->sample_aspect_ratio.num = asf->asf_sd[i].aspect_ratio.num;
                st->sample_aspect_ratio.den = asf->asf_sd[i].aspect_ratio.den;
            }
        }
    }
    return 0;
failed:
    asf_read_close(s);
    return ret;
}

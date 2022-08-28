static int asf_read_language_list(AVFormatContext *s, const GUIDParseTable *g)
{
    ASFContext *asf   = s->priv_data;
    AVIOContext *pb   = s->pb;
    int i, ret;
    uint64_t size     = avio_rl64(pb);
    uint16_t nb_langs = avio_rl16(pb);
    if (nb_langs < ASF_MAX_STREAMS) {
        for (i = 0; i < nb_langs; i++) {
            size_t len;
            len = avio_r8(pb);
            if (!len)
                len = 6;
            if ((ret = get_asf_string(pb, len, asf->asf_sd[i].langs,
                                      sizeof(asf->asf_sd[i].langs))) < 0) {
                return ret;
            }
        }
    }
    align_position(pb, asf->offset, size);
    return 0;
}

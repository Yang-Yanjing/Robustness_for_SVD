static int asf_read_unknown(AVFormatContext *s, const GUIDParseTable *g)
{
    ASFContext *asf = s->priv_data;
    AVIOContext *pb = s->pb;
    uint64_t size   = avio_rl64(pb);
    int ret;
    if (asf->is_header)
        asf->unknown_size = size;
    asf->is_header = 0;
    if (!g->is_subobject) {
        if (!(ret = strcmp(g->name, "Header Extension")))
            avio_skip(pb, 22); 
        if ((ret = detect_unknown_subobject(s, asf->unknown_offset,
                                            asf->unknown_size)) < 0)
            return ret;
    } else
        avio_skip(pb, size - 24);
    return 0;
}

static int ebml_parse(MatroskaDemuxContext *matroska, EbmlSyntax *syntax,
                      void *data)
{
    if (!matroska->current_id) {
        uint64_t id;
        int res = ebml_read_num(matroska, matroska->ctx->pb, 4, &id);
        if (res < 0) {
            
            return (matroska->is_live && matroska->ctx->pb->eof_reached &&
                    res == AVERROR_EOF) ? 1 : res;
        }
        matroska->current_id = id | 1 << 7 * res;
    }
    return ebml_parse_id(matroska, syntax, matroska->current_id, data);
}

static int ebml_parse_nest(MatroskaDemuxContext *matroska, EbmlSyntax *syntax,
                           void *data)
{
    int i, res = 0;
    for (i = 0; syntax[i].id; i++)
        switch (syntax[i].type) {
        case EBML_UINT:
            *(uint64_t *) ((char *) data + syntax[i].data_offset) = syntax[i].def.u;
            break;
        case EBML_FLOAT:
            *(double *) ((char *) data + syntax[i].data_offset) = syntax[i].def.f;
            break;
        case EBML_STR:
        case EBML_UTF8:
            
            if (syntax[i].def.s) {
                uint8_t **dst = (uint8_t **) ((uint8_t *) data + syntax[i].data_offset);
                *dst = av_strdup(syntax[i].def.s);
                if (!*dst)
                    return AVERROR(ENOMEM);
            }
            break;
        }
    while (!res && !ebml_level_end(matroska))
        res = ebml_parse(matroska, syntax, data);
    return res;
}

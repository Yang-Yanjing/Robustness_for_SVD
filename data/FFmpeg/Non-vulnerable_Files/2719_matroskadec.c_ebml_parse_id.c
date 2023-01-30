                           EbmlSyntax *syntax, void *data);
static int ebml_parse_id(MatroskaDemuxContext *matroska, EbmlSyntax *syntax,
                         uint32_t id, void *data)
{
    int i;
    for (i = 0; syntax[i].id; i++)
        if (id == syntax[i].id)
            break;
    if (!syntax[i].id && id == MATROSKA_ID_CLUSTER &&
        matroska->num_levels > 0                   &&
        matroska->levels[matroska->num_levels - 1].length == 0xffffffffffffff)
        return 0;  
    if (!syntax[i].id && id != EBML_ID_VOID && id != EBML_ID_CRC32) {
        av_log(matroska->ctx, AV_LOG_DEBUG, "Unknown entry 0x%"PRIX32"\n", id);
    }
    return ebml_parse_elem(matroska, &syntax[i], data);
}

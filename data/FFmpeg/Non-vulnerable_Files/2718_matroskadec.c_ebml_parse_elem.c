static int ebml_parse_elem(MatroskaDemuxContext *matroska,
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
}
static int ebml_parse_elem(MatroskaDemuxContext *matroska,
                           EbmlSyntax *syntax, void *data)
{
    static const uint64_t max_lengths[EBML_TYPE_COUNT] = {
        [EBML_UINT]  = 8,
        [EBML_FLOAT] = 8,
        
        [EBML_STR]   = 0x1000000,
        [EBML_UTF8]  = 0x1000000,
        
        [EBML_BIN]   = 0x10000000,
        
    };
    AVIOContext *pb = matroska->ctx->pb;
    uint32_t id = syntax->id;
    uint64_t length;
    int res;
    void *newelem;
    MatroskaLevel1Element *level1_elem;
    data = (char *) data + syntax->data_offset;
    if (syntax->list_elem_size) {
        EbmlList *list = data;
        newelem = av_realloc_array(list->elem, list->nb_elem + 1, syntax->list_elem_size);
        if (!newelem)
            return AVERROR(ENOMEM);
        list->elem = newelem;
        data = (char *) list->elem + list->nb_elem * syntax->list_elem_size;
        memset(data, 0, syntax->list_elem_size);
        list->nb_elem++;
    }
    if (syntax->type != EBML_PASS && syntax->type != EBML_STOP) {
        matroska->current_id = 0;
        if ((res = ebml_read_length(matroska, pb, &length)) < 0)
            return res;
        if (max_lengths[syntax->type] && length > max_lengths[syntax->type]) {
            av_log(matroska->ctx, AV_LOG_ERROR,
                   "Invalid length 0x%"PRIx64" > 0x%"PRIx64" for syntax element %i\n",
                   length, max_lengths[syntax->type], syntax->type);
            return AVERROR_INVALIDDATA;
        }
    }
    switch (syntax->type) {
    case EBML_UINT:
        res = ebml_read_uint(pb, length, data);
        break;
    case EBML_SINT:
        res = ebml_read_sint(pb, length, data);
        break;
    case EBML_FLOAT:
        res = ebml_read_float(pb, length, data);
        break;
    case EBML_STR:
    case EBML_UTF8:
        res = ebml_read_ascii(pb, length, data);
        break;
    case EBML_BIN:
        res = ebml_read_binary(pb, length, data);
        break;
    case EBML_LEVEL1:
    case EBML_NEST:
        if ((res = ebml_read_master(matroska, length)) < 0)
            return res;
        if (id == MATROSKA_ID_SEGMENT)
            matroska->segment_start = avio_tell(matroska->ctx->pb);
        if (id == MATROSKA_ID_CUES)
            matroska->cues_parsing_deferred = 0;
        if (syntax->type == EBML_LEVEL1 &&
            (level1_elem = matroska_find_level1_elem(matroska, syntax->id))) {
            if (level1_elem->parsed)
                av_log(matroska->ctx, AV_LOG_ERROR, "Duplicate element\n");
            level1_elem->parsed = 1;
        }
        return ebml_parse_nest(matroska, syntax->def.n, data);
    case EBML_PASS:
        return ebml_parse_id(matroska, syntax->def.n, id, data);
    case EBML_STOP:
        return 1;
    default:
        if (ffio_limit(pb, length) != length)
            return AVERROR(EIO);
        return avio_skip(pb, length) < 0 ? AVERROR(EIO) : 0;
    }
    if (res == AVERROR_INVALIDDATA)
        av_log(matroska->ctx, AV_LOG_ERROR, "Invalid element\n");
    else if (res == AVERROR(EIO))
        av_log(matroska->ctx, AV_LOG_ERROR, "Read error\n");
    return res;
}

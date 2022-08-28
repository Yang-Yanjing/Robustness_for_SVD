static void matroska_execute_seekhead(MatroskaDemuxContext *matroska)
{
    EbmlList *seekhead_list = &matroska->seekhead;
    int i;
    
    if (!matroska->ctx->pb->seekable)
        return;
    for (i = 0; i < seekhead_list->nb_elem; i++) {
        MatroskaSeekhead *seekheads = seekhead_list->elem;
        uint32_t id  = seekheads[i].id;
        uint64_t pos = seekheads[i].pos;
        MatroskaLevel1Element *elem = matroska_find_level1_elem(matroska, id);
        if (!elem || elem->parsed)
            continue;
        elem->pos = pos;
        
        if (id == MATROSKA_ID_CUES)
            continue;
        if (matroska_parse_seekhead_entry(matroska, pos) < 0) {
            
            matroska->cues_parsing_deferred = -1;
            break;
        }
        elem->parsed = 1;
    }
}

static void matroska_parse_cues(MatroskaDemuxContext *matroska) {
    int i;
    if (matroska->ctx->flags & AVFMT_FLAG_IGNIDX)
        return;
    for (i = 0; i < matroska->num_level1_elems; i++) {
        MatroskaLevel1Element *elem = &matroska->level1_elems[i];
        if (elem->id == MATROSKA_ID_CUES && !elem->parsed) {
            if (matroska_parse_seekhead_entry(matroska, elem->pos) < 0)
                matroska->cues_parsing_deferred = -1;
            elem->parsed = 1;
            break;
        }
    }
    matroska_add_index_entries(matroska);
}

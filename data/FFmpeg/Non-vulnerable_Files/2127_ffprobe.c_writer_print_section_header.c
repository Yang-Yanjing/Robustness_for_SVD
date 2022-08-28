static inline void writer_print_section_header(WriterContext *wctx,
                                               int section_id)
{
    int parent_section_id;
    wctx->level++;
    av_assert0(wctx->level < SECTION_MAX_NB_LEVELS);
    parent_section_id = wctx->level ?
        (wctx->section[wctx->level-1])->id : SECTION_ID_NONE;
    wctx->nb_item[wctx->level] = 0;
    wctx->section[wctx->level] = &wctx->sections[section_id];
    if (section_id == SECTION_ID_PACKETS_AND_FRAMES) {
        wctx->nb_section_packet = wctx->nb_section_frame =
        wctx->nb_section_packet_frame = 0;
    } else if (parent_section_id == SECTION_ID_PACKETS_AND_FRAMES) {
        wctx->nb_section_packet_frame = section_id == SECTION_ID_PACKET ?
            wctx->nb_section_packet : wctx->nb_section_frame;
    }
    if (wctx->writer->print_section_header)
        wctx->writer->print_section_header(wctx);
}

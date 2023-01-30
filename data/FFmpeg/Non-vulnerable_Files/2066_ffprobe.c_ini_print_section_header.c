static void ini_print_section_header(WriterContext *wctx)
{
    INIContext *ini = wctx->priv;
    AVBPrint *buf = &wctx->section_pbuf[wctx->level];
    const struct section *section = wctx->section[wctx->level];
    const struct section *parent_section = wctx->level ?
        wctx->section[wctx->level-1] : NULL;
    av_bprint_clear(buf);
    if (!parent_section) {
        printf("# ffprobe output\n\n");
        return;
    }
    if (wctx->nb_item[wctx->level-1])
        printf("\n");
    av_bprintf(buf, "%s", wctx->section_pbuf[wctx->level-1].str);
    if (ini->hierarchical ||
        !(section->flags & (SECTION_FLAG_IS_ARRAY|SECTION_FLAG_IS_WRAPPER))) {
        av_bprintf(buf, "%s%s", buf->str[0] ? "." : "", wctx->section[wctx->level]->name);
        if (parent_section->flags & SECTION_FLAG_IS_ARRAY) {
            int n = parent_section->id == SECTION_ID_PACKETS_AND_FRAMES ?
                wctx->nb_section_packet_frame : wctx->nb_item[wctx->level-1];
            av_bprintf(buf, ".%d", n);
        }
    }
    if (!(section->flags & (SECTION_FLAG_IS_ARRAY|SECTION_FLAG_IS_WRAPPER)))
        printf("[%s]\n", buf->str);
}

static void compact_print_section_header(WriterContext *wctx)
{
    CompactContext *compact = wctx->priv;
    const struct section *section = wctx->section[wctx->level];
    const struct section *parent_section = wctx->level ?
        wctx->section[wctx->level-1] : NULL;
    compact->terminate_line[wctx->level] = 1;
    compact->has_nested_elems[wctx->level] = 0;
    av_bprint_clear(&wctx->section_pbuf[wctx->level]);
    if (!(section->flags & SECTION_FLAG_IS_ARRAY) && parent_section &&
        !(parent_section->flags & (SECTION_FLAG_IS_WRAPPER|SECTION_FLAG_IS_ARRAY))) {
        compact->nested_section[wctx->level] = 1;
        compact->has_nested_elems[wctx->level-1] = 1;
        av_bprintf(&wctx->section_pbuf[wctx->level], "%s%s:",
                   wctx->section_pbuf[wctx->level-1].str,
                   (char *)av_x_if_null(section->element_name, section->name));
        wctx->nb_item[wctx->level] = wctx->nb_item[wctx->level-1];
    } else {
        if (parent_section && compact->has_nested_elems[wctx->level-1] &&
            (section->flags & SECTION_FLAG_IS_ARRAY)) {
            compact->terminate_line[wctx->level-1] = 0;
            printf("\n");
        }
        if (compact->print_section &&
            !(section->flags & (SECTION_FLAG_IS_WRAPPER|SECTION_FLAG_IS_ARRAY)))
            printf("%s%c", section->name, compact->item_sep);
    }
}

static void compact_print_section_footer(WriterContext *wctx)
{
    CompactContext *compact = wctx->priv;
    if (!compact->nested_section[wctx->level] &&
        compact->terminate_line[wctx->level] &&
        !(wctx->section[wctx->level]->flags & (SECTION_FLAG_IS_WRAPPER|SECTION_FLAG_IS_ARRAY)))
        printf("\n");
}

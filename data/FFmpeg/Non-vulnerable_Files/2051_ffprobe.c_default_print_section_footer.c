static void default_print_section_footer(WriterContext *wctx)
{
    DefaultContext *def = wctx->priv;
    const struct section *section = wctx->section[wctx->level];
    char buf[32];
    if (def->noprint_wrappers || def->nested_section[wctx->level])
        return;
    if (!(section->flags & (SECTION_FLAG_IS_WRAPPER|SECTION_FLAG_IS_ARRAY)))
        printf("[/%s]\n", upcase_string(buf, sizeof(buf), section->name));
}

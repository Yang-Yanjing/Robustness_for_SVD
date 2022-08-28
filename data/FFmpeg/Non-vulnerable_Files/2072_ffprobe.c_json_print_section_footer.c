static void json_print_section_footer(WriterContext *wctx)
{
    JSONContext *json = wctx->priv;
    const struct section *section = wctx->section[wctx->level];
    if (wctx->level == 0) {
        json->indent_level--;
        printf("\n}\n");
    } else if (section->flags & SECTION_FLAG_IS_ARRAY) {
        printf("\n");
        json->indent_level--;
        JSON_INDENT();
        printf("]");
    } else {
        printf("%s", json->item_start_end);
        json->indent_level--;
        if (!json->compact)
            JSON_INDENT();
        printf("}");
	}
}
static void xml_print_str(WriterContext *wctx, const char *key, const char *value)
{
    AVBPrint buf;
    XMLContext *xml = wctx->priv;
    const struct section *section = wctx->section[wctx->level];
    av_bprint_init(&buf, 1, AV_BPRINT_SIZE_UNLIMITED);
    if (section->flags & SECTION_FLAG_HAS_VARIABLE_FIELDS) {
        XML_INDENT();
        printf("<%s key=\"%s\"",
               section->element_name, xml_escape_str(&buf, key, wctx));
        av_bprint_clear(&buf);
        printf(" value=\"%s\"/>\n", xml_escape_str(&buf, value, wctx));
    } else {
        if (wctx->nb_item[wctx->level])
            printf(" ");
        printf("%s=\"%s\"", key, xml_escape_str(&buf, value, wctx));
    }
    av_bprint_finalize(&buf, NULL);
}

static void xml_print_section_header(WriterContext *wctx)
{
    XMLContext *xml = wctx->priv;
    const struct section *section = wctx->section[wctx->level];
    const struct section *parent_section = wctx->level ?
        wctx->section[wctx->level-1] : NULL;
    if (wctx->level == 0) {
        const char *qual = " xmlns:xsi='http://www.w3.org/2001/XMLSchema-instance' "
            "xmlns:ffprobe='http://www.ffmpeg.org/schema/ffprobe' "
            "xsi:schemaLocation='http://www.ffmpeg.org/schema/ffprobe ffprobe.xsd'";
        printf("<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n");
        printf("<%sffprobe%s>\n",
               xml->fully_qualified ? "ffprobe:" : "",
               xml->fully_qualified ? qual : "");
        return;
    }
    if (xml->within_tag) {
        xml->within_tag = 0;
        printf(">\n");
    }
    if (section->flags & SECTION_FLAG_HAS_VARIABLE_FIELDS) {
        xml->indent_level++;
    } else {
        if (parent_section && (parent_section->flags & SECTION_FLAG_IS_WRAPPER) &&
            wctx->level && wctx->nb_item[wctx->level-1])
            printf("\n");
        xml->indent_level++;
        if (section->flags & SECTION_FLAG_IS_ARRAY) {
            XML_INDENT(); printf("<%s>\n", section->name);
        } else {
            XML_INDENT(); printf("<%s ", section->name);
            xml->within_tag = 1;
        }
    }
}

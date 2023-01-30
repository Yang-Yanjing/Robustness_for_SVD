static int attachment_is_font(AVStream * st)
{
    const AVDictionaryEntry *tag = NULL;
    int n;
    tag = av_dict_get(st->metadata, "mimetype", NULL, AV_DICT_MATCH_CASE);
    if (tag) {
        for (n = 0; font_mimetypes[n]; n++) {
            if (av_strcasecmp(font_mimetypes[n], tag->value) == 0)
                return 1;
        }
    }
    return 0;
}

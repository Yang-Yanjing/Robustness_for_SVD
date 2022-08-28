static inline int show_tags(WriterContext *w, AVDictionary *tags, int section_id)
{
    AVDictionaryEntry *tag = NULL;
    int ret = 0;
    if (!tags)
        return 0;
    writer_print_section_header(w, section_id);
    while ((tag = av_dict_get(tags, "", tag, AV_DICT_IGNORE_SUFFIX))) {
        if ((ret = print_str_validate(tag->key, tag->value)) < 0)
            break;
    }
    writer_print_section_footer(w);
    return ret;
}

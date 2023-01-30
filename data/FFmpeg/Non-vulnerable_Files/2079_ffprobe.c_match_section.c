static int match_section(const char *section_name,
                         int show_all_entries, AVDictionary *entries)
{
    int i, ret = 0;
    for (i = 0; i < FF_ARRAY_ELEMS(sections); i++) {
        const struct section *section = &sections[i];
        if (!strcmp(section_name, section->name) ||
            (section->unique_name && !strcmp(section_name, section->unique_name))) {
            av_log(NULL, AV_LOG_DEBUG,
                   "'%s' matches section with unique name '%s'\n", section_name,
                   (char *)av_x_if_null(section->unique_name, section->name));
            ret++;
            mark_section_show_entries(section->id, show_all_entries, entries);
        }
    }
    return ret;
}

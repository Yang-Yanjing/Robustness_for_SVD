static inline void mark_section_show_entries(SectionID section_id,
                                             int show_all_entries, AVDictionary *entries)
{
    struct section *section = &sections[section_id];
    section->show_all_entries = show_all_entries;
    if (show_all_entries) {
        SectionID *id;
        for (id = section->children_ids; *id != -1; id++)
            mark_section_show_entries(*id, show_all_entries, entries);
    } else {
        av_dict_copy(&section->entries_to_show, entries, 0);
    }
}

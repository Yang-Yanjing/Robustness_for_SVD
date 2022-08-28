static inline int check_section_show_entries(int section_id)
{
    int *id;
    struct section *section = &sections[section_id];
    if (sections[section_id].show_all_entries || sections[section_id].entries_to_show)
        return 1;
    for (id = section->children_ids; *id != -1; id++)
        if (check_section_show_entries(*id))
            return 1;
    return 0;
}

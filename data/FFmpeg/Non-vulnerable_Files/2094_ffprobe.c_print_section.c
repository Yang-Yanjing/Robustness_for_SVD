static void print_section(SectionID id, int level)
{
    const SectionID *pid;
    const struct section *section = &sections[id];
    printf("%c%c%c",
           section->flags & SECTION_FLAG_IS_WRAPPER           ? 'W' : '.',
           section->flags & SECTION_FLAG_IS_ARRAY             ? 'A' : '.',
           section->flags & SECTION_FLAG_HAS_VARIABLE_FIELDS  ? 'V' : '.');
    printf("%*c  %s", level * 4, ' ', section->name);
    if (section->unique_name)
        printf("/%s", section->unique_name);
    printf("\n");
    for (pid = section->children_ids; *pid != -1; pid++)
        print_section(*pid, level+1);
}

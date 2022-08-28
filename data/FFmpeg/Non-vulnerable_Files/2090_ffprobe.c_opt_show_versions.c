static int opt_show_versions(const char *opt, const char *arg)
{
    mark_section_show_entries(SECTION_ID_PROGRAM_VERSION, 1, NULL);
    mark_section_show_entries(SECTION_ID_LIBRARY_VERSION, 1, NULL);
    return 0;
}

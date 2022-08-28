static int opt_sections(void *optctx, const char *opt, const char *arg)
{
    printf("Sections:\n"
           "W.. = Section is a wrapper (contains other sections, no local entries)\n"
           ".A. = Section contains an array of elements of the same type\n"
           "..V = Section may contain a variable number of fields with variable keys\n"
           "FLAGS NAME/UNIQUE_NAME\n"
           "---\n");
    print_section(SECTION_ID_ROOT, 0);
    return 0;
}

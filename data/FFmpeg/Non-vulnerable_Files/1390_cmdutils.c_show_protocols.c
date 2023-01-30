int show_protocols(void *optctx, const char *opt, const char *arg)
{
    void *opaque = NULL;
    const char *name;
    printf("Supported file protocols:\n"
           "Input:\n");
    while ((name = avio_enum_protocols(&opaque, 0)))
        printf("  %s\n", name);
    printf("Output:\n");
    while ((name = avio_enum_protocols(&opaque, 1)))
        printf("  %s\n", name);
    return 0;
}

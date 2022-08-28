int show_layouts(void *optctx, const char *opt, const char *arg)
{
    int i = 0;
    uint64_t layout, j;
    const char *name, *descr;
    printf("Individual channels:\n"
           "NAME           DESCRIPTION\n");
    for (i = 0; i < 63; i++) {
        name = av_get_channel_name((uint64_t)1 << i);
        if (!name)
            continue;
        descr = av_get_channel_description((uint64_t)1 << i);
        printf("%-14s %s\n", name, descr);
    }
    printf("\nStandard channel layouts:\n"
           "NAME           DECOMPOSITION\n");
    for (i = 0; !av_get_standard_channel_layout(i, &layout, &name); i++) {
        if (name) {
            printf("%-14s ", name);
            for (j = 1; j; j <<= 1)
                if ((layout & j))
                    printf("%s%s", (layout & (j - 1)) ? "+" : "", av_get_channel_name(j));
            printf("\n");
        }
    }
    return 0;
}

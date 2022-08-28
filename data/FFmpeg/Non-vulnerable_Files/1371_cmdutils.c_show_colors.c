int show_colors(void *optctx, const char *opt, const char *arg)
{
    const char *name;
    const uint8_t *rgb;
    int i;
    printf("%-32s #RRGGBB\n", "name");
    for (i = 0; name = av_get_known_color_name(i, &rgb); i++)
        printf("%-32s #%02x%02x%02x\n", name, rgb[0], rgb[1], rgb[2]);
    return 0;
}

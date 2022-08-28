int main(int argc, char **argv)
{
    int i, list_fourcc_pix_fmt = 0, list_pix_fmt_fourccs = 0;
    const PixelFormatTag *pix_fmt_tags = avpriv_get_raw_pix_fmt_tags();
    const char *pix_fmt_name = NULL;
    char c;
    if (argc == 1) {
        usage();
        return 0;
    }
    while ((c = getopt(argc, argv, "hp:lL")) != -1) {
        switch (c) {
        case 'h':
            usage();
            return 0;
        case 'l':
            list_fourcc_pix_fmt = 1;
            break;
        case 'L':
            list_pix_fmt_fourccs = 1;
            break;
        case 'p':
            pix_fmt_name = optarg;
            break;
        case '?':
            usage();
            return 1;
        }
    }
    if (list_fourcc_pix_fmt) {
        for (i = 0; pix_fmt_tags[i].pix_fmt != AV_PIX_FMT_NONE; i++) {
            char buf[32];
            av_get_codec_tag_string(buf, sizeof(buf), pix_fmt_tags[i].fourcc);
            printf("%s: %s\n", buf, av_get_pix_fmt_name(pix_fmt_tags[i].pix_fmt));
        }
    }
    if (list_pix_fmt_fourccs) {
        for (i = 0; av_pix_fmt_desc_get(i); i++) {
            const AVPixFmtDescriptor *pix_desc = av_pix_fmt_desc_get(i);
            if (!pix_desc->name || pix_desc->flags & AV_PIX_FMT_FLAG_HWACCEL)
                continue;
            printf("%s: ", pix_desc->name);
            print_pix_fmt_fourccs(i, pix_fmt_tags, ' ');
            printf("\n");
        }
    }
    if (pix_fmt_name) {
        enum AVPixelFormat pix_fmt = av_get_pix_fmt(pix_fmt_name);
        if (pix_fmt == AV_PIX_FMT_NONE) {
            fprintf(stderr, "Invalid pixel format selected '%s'\n", pix_fmt_name);
            return 1;
        }
        print_pix_fmt_fourccs(pix_fmt, pix_fmt_tags, '\n');
    }
    return 0;
}

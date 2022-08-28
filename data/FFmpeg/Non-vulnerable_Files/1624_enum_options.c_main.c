int main(int argc, char **argv)
{
    if (argc < 2)
        print_usage();
    av_register_all();
    if (!strcmp(argv[1], "format"))
        show_format_opts();
    else if (!strcmp(argv[1], "codec"))
        show_codec_opts();
    else
        print_usage();
    return 0;
}

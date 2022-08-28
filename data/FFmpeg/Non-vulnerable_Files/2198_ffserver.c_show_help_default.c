void show_help_default(const char *opt, const char *arg)
{
    printf("usage: ffserver [options]\n"
           "Hyper fast multi format Audio/Video streaming server\n");
    printf("\n");
    show_help_options(options, "Main options:", 0, 0, 0);
}

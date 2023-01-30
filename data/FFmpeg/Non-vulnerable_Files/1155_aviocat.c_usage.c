static int usage(const char *argv0, int ret)
{
    fprintf(stderr, "%s [-b bytespersec] [-d duration] input_url output_url\n", argv0);
    return ret;
}

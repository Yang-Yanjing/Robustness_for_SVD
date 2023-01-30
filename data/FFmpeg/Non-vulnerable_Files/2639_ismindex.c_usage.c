static int usage(const char *argv0, int ret)
{
    fprintf(stderr, "%s [-split] [-ismf] [-n basename] [-path-prefix prefix] "
                    "[-ismc-prefix prefix] [-output dir] file1 [file2] ...\n", argv0);
    return ret;
}

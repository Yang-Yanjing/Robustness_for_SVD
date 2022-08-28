static void usage(int ret)
{
    fprintf(ret ? stderr : stdout,
            "Usage: seek_print file [command ...]\n"
            "Commands:\n"
            "    read\n"
            "    seek:stream:min_ts:ts:max_ts:flags\n"
            );
    exit(ret);
}

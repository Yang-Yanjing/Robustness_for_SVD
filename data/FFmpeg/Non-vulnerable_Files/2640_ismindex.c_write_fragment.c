static int write_fragment(const char *filename, AVIOContext *in)
{
    AVIOContext *out = NULL;
    int ret;
    if ((ret = avio_open2(&out, filename, AVIO_FLAG_WRITE, NULL, NULL)) < 0) {
        char errbuf[100];
        av_strerror(ret, errbuf, sizeof(errbuf));
        fprintf(stderr, "Unable to open %s: %s\n", filename, errbuf);
        return ret;
    }
    ret = copy_tag(in, out, MKBETAG('m', 'o', 'o', 'f'));
    if (!ret)
        ret = copy_tag(in, out, MKBETAG('m', 'd', 'a', 't'));
    avio_flush(out);
    avio_close(out);
    return ret;
}

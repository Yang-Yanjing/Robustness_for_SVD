static int skip_fragment(AVIOContext *in)
{
    int ret;
    ret = skip_tag(in, MKBETAG('m', 'o', 'o', 'f'));
    if (!ret)
        ret = skip_tag(in, MKBETAG('m', 'd', 'a', 't'));
    return ret;
}

static int open_url(HLSContext *c, URLContext **uc, const char *url, AVDictionary *opts)
{
    AVDictionary *tmp = NULL;
    int ret;
    av_dict_copy(&tmp, c->avio_opts, 0);
    av_dict_copy(&tmp, opts, 0);
    ret = ffurl_open(uc, url, AVIO_FLAG_READ, c->interrupt_callback, &tmp);
    av_dict_free(&tmp);
    return ret;
}

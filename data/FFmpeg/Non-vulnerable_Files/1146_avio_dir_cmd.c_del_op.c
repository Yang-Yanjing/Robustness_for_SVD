static int del_op(const char *url)
{
    int ret = avpriv_io_delete(url);
    if (ret < 0)
        av_log(NULL, AV_LOG_ERROR, "Cannot delete '%s': %s.\n", url, av_err2str(ret));
    return ret;
}

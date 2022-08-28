static int move_op(const char *src, const char *dst)
{
    int ret = avpriv_io_move(src, dst);
    if (ret < 0)
        av_log(NULL, AV_LOG_ERROR, "Cannot move '%s' into '%s': %s.\n", src, dst, av_err2str(ret));
    return ret;
}

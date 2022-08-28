int ff_framesync_filter_frame(FFFrameSync *fs, AVFilterLink *inlink,
                              AVFrame *in)
{
    int ret;
    if ((ret = ff_framesync_process_frame(fs, 1)) < 0)
        return ret;
    if ((ret = ff_framesync_add_frame(fs, FF_INLINK_IDX(inlink), in)) < 0)
        return ret;
    if ((ret = ff_framesync_process_frame(fs, 0)) < 0)
        return ret;
    return 0;
}

int ff_framesync_process_frame(FFFrameSync *fs, unsigned all)
{
    int ret, count = 0;
    av_assert0(fs->on_event);
    while (1) {
        ff_framesync_next(fs);
        if (fs->eof || !fs->frame_ready)
            break;
        if ((ret = fs->on_event(fs)) < 0)
            return ret;
        ff_framesync_drop(fs);
        count++;
        if (!all)
            break;
    }
    if (!count && fs->eof)
        return AVERROR_EOF;
    return count;
}

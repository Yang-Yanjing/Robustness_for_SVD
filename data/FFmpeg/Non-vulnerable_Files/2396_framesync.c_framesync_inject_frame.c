static void framesync_inject_frame(FFFrameSync *fs, unsigned in, AVFrame *frame)
{
    int64_t pts;
    av_assert0(!fs->in[in].have_next);
    if (frame) {
        pts = av_rescale_q(frame->pts, fs->in[in].time_base, fs->time_base);
        frame->pts = pts;
    } else {
        pts = fs->in[in].state != STATE_RUN || fs->in[in].after == EXT_INFINITY
            ? INT64_MAX : framesync_pts_extrapolate(fs, in, fs->in[in].pts);
        fs->in[in].sync = 0;
        framesync_sync_level_update(fs);
    }
    fs->in[in].frame_next = frame;
    fs->in[in].pts_next   = pts;
    fs->in[in].have_next  = 1;
}

int ff_framesync_get_frame(FFFrameSync *fs, unsigned in, AVFrame **rframe,
                           unsigned get)
{
    AVFrame *frame;
    unsigned need_copy = 0, i;
    int64_t pts_next;
    int ret;
    if (!fs->in[in].frame) {
        *rframe = NULL;
        return 0;
    }
    frame = fs->in[in].frame;
    if (get) {
        

        pts_next = fs->in[in].have_next ? fs->in[in].pts_next : INT64_MAX;
        for (i = 0; i < fs->nb_in && !need_copy; i++)
            if (i != in && fs->in[i].sync &&
                (!fs->in[i].have_next || fs->in[i].pts_next < pts_next))
                need_copy = 1;
        if (need_copy) {
            if (!(frame = av_frame_clone(frame)))
                return AVERROR(ENOMEM);
            if ((ret = av_frame_make_writable(frame)) < 0) {
                av_frame_free(&frame);
                return ret;
            }
        } else {
            fs->in[in].frame = NULL;
        }
        fs->frame_ready = 0;
    }
    *rframe = frame;
    return 0;
}

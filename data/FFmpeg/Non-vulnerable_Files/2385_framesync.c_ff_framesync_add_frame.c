int ff_framesync_add_frame(FFFrameSync *fs, unsigned in, AVFrame *frame)
{
    av_assert1(in < fs->nb_in);
    if (!fs->in[in].have_next)
        framesync_inject_frame(fs, in, frame);
    else
        ff_bufqueue_add(fs, &fs->in[in].queue, frame);
    return 0;
}

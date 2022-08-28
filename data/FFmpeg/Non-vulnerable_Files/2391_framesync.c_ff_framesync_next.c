void ff_framesync_next(FFFrameSync *fs)
{
    unsigned i;
    av_assert0(!fs->frame_ready);
    for (i = 0; i < fs->nb_in; i++)
        if (!fs->in[i].have_next && fs->in[i].queue.available)
            framesync_inject_frame(fs, i, ff_bufqueue_get(&fs->in[i].queue));
    fs->frame_ready = 0;
    framesync_advance(fs);
}

void ff_framesync_uninit(FFFrameSync *fs)
{
    unsigned i;
    for (i = 0; i < fs->nb_in; i++) {
        av_frame_free(&fs->in[i].frame);
        av_frame_free(&fs->in[i].frame_next);
        ff_bufqueue_discard_all(&fs->in[i].queue);
    }
    av_freep(&fs->in);
}

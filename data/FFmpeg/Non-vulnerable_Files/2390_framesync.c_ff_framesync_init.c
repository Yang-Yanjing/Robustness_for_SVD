int ff_framesync_init(FFFrameSync *fs, void *parent, unsigned nb_in)
{
    fs->class  = &framesync_class;
    fs->parent = parent;
    fs->nb_in  = nb_in;
    fs->in = av_calloc(nb_in, sizeof(*fs->in));
    if (!fs->in)
        return AVERROR(ENOMEM);
    return 0;
}

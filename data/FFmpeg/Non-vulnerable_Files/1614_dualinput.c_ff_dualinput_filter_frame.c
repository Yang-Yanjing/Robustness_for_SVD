int ff_dualinput_filter_frame(FFDualInputContext *s,
                                   AVFilterLink *inlink, AVFrame *in)
{
    return ff_framesync_filter_frame(&s->fs, inlink, in);
}

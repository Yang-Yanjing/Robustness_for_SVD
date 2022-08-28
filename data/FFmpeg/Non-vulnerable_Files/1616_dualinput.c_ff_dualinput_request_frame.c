int ff_dualinput_request_frame(FFDualInputContext *s, AVFilterLink *outlink)
{
    return ff_framesync_request_frame(&s->fs, outlink);
}

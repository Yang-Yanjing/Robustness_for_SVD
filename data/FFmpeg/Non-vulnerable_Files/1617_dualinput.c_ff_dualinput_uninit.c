void ff_dualinput_uninit(FFDualInputContext *s)
{
    ff_framesync_uninit(&s->fs);
}

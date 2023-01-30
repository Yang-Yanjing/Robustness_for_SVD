}
static void Close(vlc_object_t *obj)
{
    audio_output_t *aout = (audio_output_t *)obj;
    aout_sys_t *sys = aout->sys;
    vlc_mutex_destroy(&sys->lock);
    vlc_cond_destroy(&sys->cond);
    free(sys);
}

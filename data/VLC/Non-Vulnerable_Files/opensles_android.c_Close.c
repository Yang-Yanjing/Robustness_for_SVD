 *****************************************************************************/
static void Close(vlc_object_t *obj)
{
    audio_output_t *aout = (audio_output_t *)obj;
    aout_sys_t *sys = aout->sys;
    Destroy(sys->outputMixObject);
    Destroy(sys->engineObject);
    dlclose(sys->p_so_handle);
    vlc_mutex_destroy(&sys->lock);
    free(sys);
}

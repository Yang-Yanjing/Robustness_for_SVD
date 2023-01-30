}
static void Close(vlc_object_t *obj)
{
    audio_output_t *aout = (audio_output_t *)obj;
    aout_sys_t *sys = aout->sys;
    var_Destroy(aout, "directx-audio-device");
    FreeLibrary(sys->hdsound_dll); /* free DSOUND.DLL */
    free(sys);
}

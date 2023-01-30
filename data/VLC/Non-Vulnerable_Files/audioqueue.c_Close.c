}
static void Close(vlc_object_t *obj)
{
    audio_output_t *aout = (audio_output_t *)obj;
    msg_Dbg( aout, "audioqueue: Close");
    aout_sys_t *sys = aout->sys;
    free(sys);
}

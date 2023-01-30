}
static void Stop(audio_output_t* p_aout)
{
    aout_sys_t *p_sys = p_aout->sys;
    p_sys->at_stop(p_sys->AudioTrack);
    p_sys->at_flush(p_sys->AudioTrack);
    p_sys->at_dtor(p_sys->AudioTrack);
    free(p_sys->AudioTrack);
}

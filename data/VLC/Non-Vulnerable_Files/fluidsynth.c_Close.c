}
static void Close (vlc_object_t *p_this)
{
    decoder_sys_t *p_sys = ((decoder_t *)p_this)->p_sys;
    fluid_synth_sfunload (p_sys->synth, p_sys->soundfont, 1);
    delete_fluid_synth (p_sys->synth);
    delete_fluid_settings (p_sys->settings);
    free (p_sys);
}

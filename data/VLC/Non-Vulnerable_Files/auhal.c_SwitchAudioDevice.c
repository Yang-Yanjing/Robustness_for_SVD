}
static int SwitchAudioDevice(audio_output_t *p_aout, const char *name)
{
    struct aout_sys_t *p_sys = p_aout->sys;
    if (name)
        p_sys->i_new_selected_dev = atoi(name);
    else
        p_sys->i_new_selected_dev = 0;
    bool b_supports_digital = (p_sys->i_new_selected_dev & AOUT_VAR_SPDIF_FLAG);
    if (b_supports_digital)
        p_sys->b_selected_dev_is_digital = true;
    else
        p_sys->b_selected_dev_is_digital = false;
    p_sys->i_new_selected_dev = p_sys->i_new_selected_dev & ~AOUT_VAR_SPDIF_FLAG;
    aout_DeviceReport(p_aout, name);
    aout_RestartRequest(p_aout, AOUT_RESTART_OUTPUT);
    return 0;
}

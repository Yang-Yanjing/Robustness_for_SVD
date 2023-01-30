}
static void Stop(audio_output_t *p_aout)
{
    struct aout_sys_t   *p_sys = p_aout->sys;
    OSStatus status;
    AudioSessionSetActive(false);
    if (p_sys->au_unit) {
        status = AudioOutputUnitStop(p_sys->au_unit);
        if (status != noErr)
            msg_Warn(p_aout, "failed to stop AudioUnit (%i)", (int)status);
        status = AudioUnitUninitialize(p_sys->au_unit);
        if (status != noErr)
            msg_Warn(p_aout, "failed to uninit AudioUnit (%i)", (int)status);
        status = AudioComponentInstanceDispose(p_sys->au_unit);
        if (status != noErr)
            msg_Warn(p_aout, "failed to dispose Audio Component instance (%i)", (int)status);
    }
    p_sys->i_bytes_per_sample = 0;
    /* clean-up circular buffer */
    TPCircularBufferCleanup(&p_sys->circular_buffer);
}

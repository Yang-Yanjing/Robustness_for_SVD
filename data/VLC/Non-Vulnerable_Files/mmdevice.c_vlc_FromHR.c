 * and (trivially) the device and session notifications. */
static int vlc_FromHR(audio_output_t *aout, HRESULT hr)
{
    /* Restart on unplug */
    if (unlikely(hr == AUDCLNT_E_DEVICE_INVALIDATED))
        aout_RestartRequest(aout, AOUT_RESTART_OUTPUT);
    return SUCCEEDED(hr) ? 0 : -1;
}

}
static void Play(audio_output_t *aout, block_t *block)
{
    aout_sys_t *sys = aout->sys;
    HRESULT hr;
    EnterMTA();
    hr = aout_stream_Play(sys->stream, block);
    LeaveMTA();
    vlc_FromHR(aout, hr);
}

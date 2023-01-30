}
static void Pause(audio_output_t *aout, bool paused, mtime_t date)
{
    aout_sys_t *sys = aout->sys;
    HRESULT hr;
    EnterMTA();
    hr = aout_stream_Pause(sys->stream, paused);
    LeaveMTA();
    vlc_FromHR(aout, hr);
    (void) date;
}

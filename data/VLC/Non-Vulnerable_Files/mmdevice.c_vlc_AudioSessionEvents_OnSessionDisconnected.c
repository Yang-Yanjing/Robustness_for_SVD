static STDMETHODIMP
vlc_AudioSessionEvents_OnSessionDisconnected(IAudioSessionEvents *this,
                                           AudioSessionDisconnectReason reason)
{
    aout_sys_t *sys = vlc_AudioSessionEvents_sys(this);
    audio_output_t *aout = sys->aout;
    switch (reason)
    {
        case DisconnectReasonDeviceRemoval:
            msg_Warn(aout, "session disconnected: %s", "device removed");
            break;
        case DisconnectReasonServerShutdown:
            msg_Err(aout, "session disconnected: %s", "service stopped");
            return S_OK;
        case DisconnectReasonFormatChanged:
            msg_Warn(aout, "session disconnected: %s", "format changed");
            break;
        case DisconnectReasonSessionLogoff:
            msg_Err(aout, "session disconnected: %s", "user logged off");
            return S_OK;
        case DisconnectReasonSessionDisconnected:
            msg_Err(aout, "session disconnected: %s", "session disconnected");
            return S_OK;
        case DisconnectReasonExclusiveModeOverride:
            msg_Err(aout, "session disconnected: %s", "stream overriden");
            return S_OK;
        default:
            msg_Warn(aout, "session disconnected: unknown reason %d", reason);
            return S_OK;
    }
    /* NOTE: audio decoder thread should get invalidated device and restart */
    return S_OK;
}

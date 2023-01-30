}
static int MuteSet(audio_output_t *aout, bool mute)
{
    aout_sys_t *sys = aout->sys;
    EnterCriticalSection(&sys->lock);
    sys->mute = mute;
    WakeConditionVariable(&sys->work);
    LeaveCriticalSection(&sys->lock);
    return 0;
}

}
static void Pause(audio_output_t *aout, bool pause, mtime_t date)
{
    (void)date;
    aout_sys_t *sys = aout->sys;
    SetPlayState(sys->playerPlay,
        pause ? SL_PLAYSTATE_PAUSED : SL_PLAYSTATE_PLAYING);
}

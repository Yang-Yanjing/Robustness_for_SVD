 */
static void Pause(audio_output_t *aout, bool paused, mtime_t date)
{
    aout_sys_t *sys = aout->sys;
    if( paused ) {
        sys->paused = date;
    } else {
        date -= sys->paused;
        msg_Dbg(aout, "resuming after %"PRId64" us", date);
        sys->paused = VLC_TS_INVALID;
    }
}

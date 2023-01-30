}
static void Pause (audio_output_t *aout, bool paused, mtime_t date)
{
    aout_sys_t *sys = aout->sys;
    void (*cb) (void *, int64_t) = paused ? sys->pause : sys->resume;
    if (cb != NULL)
        cb (sys->opaque, date);
}

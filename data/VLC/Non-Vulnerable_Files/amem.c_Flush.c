}
static void Flush (audio_output_t *aout, bool wait)
{
    aout_sys_t *sys = aout->sys;
    void (*cb) (void *) = wait ? sys->drain : sys->flush;
    if (cb != NULL)
        cb (sys->opaque);
}

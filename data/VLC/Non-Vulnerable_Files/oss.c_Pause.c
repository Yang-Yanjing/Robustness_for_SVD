 */
static void Pause (audio_output_t *aout, bool pause, mtime_t date)
{
    aout_sys_t *sys = aout->sys;
    int fd = sys->fd;
    (void) date;
    ioctl (fd, pause ? SNDCTL_DSP_SILENCE : SNDCTL_DSP_SKIP, NULL);
}

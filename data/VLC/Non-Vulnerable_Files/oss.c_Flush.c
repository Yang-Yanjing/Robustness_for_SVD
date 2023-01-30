 */
static void Flush (audio_output_t *aout, bool wait)
{
    aout_sys_t *sys = aout->sys;
    int fd = sys->fd;
    if (wait)
        return; /* drain is implicit with OSS */
    ioctl (fd, SNDCTL_DSP_HALT, NULL);
}

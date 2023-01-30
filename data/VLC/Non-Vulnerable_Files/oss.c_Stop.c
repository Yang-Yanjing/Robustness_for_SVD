 */
static void Stop (audio_output_t *aout)
{
    aout_sys_t *sys = aout->sys;
    int fd = sys->fd;
    ioctl (fd, SNDCTL_DSP_HALT, NULL);
    close (fd);
    sys->fd = -1;
}

}
static int DevicesEnum (audio_output_t *aout)
{
    int fd = vlc_open ("/dev/dsp", O_WRONLY);
    if (fd == -1)
        return -1;
    oss_sysinfo si;
    int n = -1;
    if (ioctl (fd, SNDCTL_SYSINFO, &si) < 0)
    {
        msg_Err (aout, "cannot get system infos: %s", vlc_strerror(errno));
        goto out;
    }
    msg_Dbg (aout, "using %s version %s (0x%06X) under %s", si.product,
             si.version, si.versionnum, si.license);
    for (int i = 0; i < si.numaudios; i++)
    {
        oss_audioinfo ai = { .dev = i };
        if (ioctl (fd, SNDCTL_AUDIOINFO, &ai) < 0)
        {
            msg_Warn (aout, "cannot get device %d infos: %s", i,
                      vlc_strerror_c(errno));
            continue;
        }
        if (ai.caps & (PCM_CAP_HIDDEN|PCM_CAP_MODEM))
            continue;
        if (!(ai.caps & PCM_CAP_OUTPUT))
            continue;
        if (!ai.enabled)
            continue;
        aout_HotplugReport (aout, ai.devnode, ai.name);
        n++;
    }
out:
    close (fd);
    return n;
}

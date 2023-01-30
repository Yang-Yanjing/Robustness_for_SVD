}
static void test_audio_output (void)
{
    libvlc_instance_t *vlc = libvlc_new (0, NULL);
    assert (vlc != NULL);
    libvlc_audio_output_t *mods = libvlc_audio_output_list_get (vlc);
    assert (mods != NULL);
    puts ("Audio outputs:");
    for (const libvlc_audio_output_t *o = mods; o != NULL; o = o->p_next)
    {
        libvlc_audio_output_device_t *devs;
        printf(" %s: %s\n", o->psz_name, o->psz_description);
        devs = libvlc_audio_output_device_list_get (vlc, o->psz_name);
        if (devs == NULL)
            continue;
        for (const libvlc_audio_output_device_t *d = devs;
             d != NULL;
             d = d->p_next)
             printf("  %s: %s\n", d->psz_device, d->psz_description);
        libvlc_audio_output_device_list_release (devs);
    }
    libvlc_audio_output_list_release (mods);
    libvlc_release (vlc);
}

}
static void DumpDeviceStatus (vlc_object_t *obj, snd_pcm_t *pcm)
{
    snd_pcm_status_t *status;
    snd_pcm_status_alloca (&status);
    snd_pcm_status (pcm, status);
    Dump (obj, "current status:\n", snd_pcm_status_dump, status);
}

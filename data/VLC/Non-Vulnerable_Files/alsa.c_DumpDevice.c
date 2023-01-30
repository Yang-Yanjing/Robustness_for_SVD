        Dump(VLC_OBJECT(o), m, (int (*)(void *, snd_output_t *))(cb), p)
static void DumpDevice (vlc_object_t *obj, snd_pcm_t *pcm)
{
    snd_pcm_info_t *info;
    Dump (obj, " ", snd_pcm_dump, pcm);
    snd_pcm_info_alloca (&info);
    if (snd_pcm_info (pcm, info) == 0)
    {
        msg_Dbg (obj, " device name   : %s", snd_pcm_info_get_name (info));
        msg_Dbg (obj, " device ID     : %s", snd_pcm_info_get_id (info));
        msg_Dbg (obj, " subdevice name: %s",
                snd_pcm_info_get_subdevice_name (info));
    }
}

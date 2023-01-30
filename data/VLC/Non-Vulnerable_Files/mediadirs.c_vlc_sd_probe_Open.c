}
static int vlc_sd_probe_Open( vlc_object_t *obj )
{
    vlc_probe_t *probe = (vlc_probe_t *)obj;
    vlc_sd_probe_Add( probe, "video_dir{longname=\"My Videos\"}",
                      N_("My Videos"), SD_CAT_MYCOMPUTER );
    vlc_sd_probe_Add( probe, "audio_dir{longname=\"My Music\"}",
                      N_("My Music"), SD_CAT_MYCOMPUTER );
    vlc_sd_probe_Add( probe, "picture_dir{longname=\"My Pictures\"}",
                      N_("My Pictures"), SD_CAT_MYCOMPUTER );
    return VLC_PROBE_CONTINUE;
}

 *****************************************************************************/
static int blurayOpen(vlc_object_t *object)
{
    demux_t *p_demux = (demux_t*)object;
    demux_sys_t *p_sys;
    const char *error_msg = NULL;
#define BLURAY_ERROR(s) do { error_msg = s; goto error; } while(0)
    if (strcmp(p_demux->psz_access, "bluray")) {
        // TODO BDMV support, once we figure out what to do in libbluray
        return VLC_EGENERIC;
    }
    /* */
    p_demux->p_sys = p_sys = calloc(1, sizeof(*p_sys));
    if (unlikely(!p_sys))
        return VLC_ENOMEM;
    p_sys->current_overlay = -1;
    p_sys->i_audio_stream = -1;
    p_sys->i_spu_stream = -1;
    p_sys->i_video_stream = -1;
    p_sys->i_still_end_time = 0;
    /* init demux info fields */
    p_demux->info.i_update    = 0;
    p_demux->info.i_title     = 0;
    p_demux->info.i_seekpoint = 0;
    TAB_INIT(p_sys->i_title, p_sys->pp_title);
    /* store current bd path */
    if (p_demux->psz_file)
        p_sys->psz_bd_path = strdup(p_demux->psz_file);
    /* If we're passed a block device, try to convert it to the mount point. */
    FindMountPoint(&p_sys->psz_bd_path);
    p_sys->bluray = bd_open(p_sys->psz_bd_path, NULL);
    if (!p_sys->bluray) {
        free(p_sys->psz_bd_path);
        free(p_sys);
        return VLC_EGENERIC;
    }
    vlc_mutex_init(&p_sys->pl_info_lock);
    /* Warning the user about AACS/BD+ */
    const BLURAY_DISC_INFO *disc_info = bd_get_disc_info(p_sys->bluray);
    /* Is it a bluray? */
    if (!disc_info->bluray_detected)
        BLURAY_ERROR(_("Path doesn't appear to be a Blu-ray"));
    msg_Info(p_demux, "First play: %i, Top menu: %i\n"
                      "HDMV Titles: %i, BD-J Titles: %i, Other: %i",
             disc_info->first_play_supported, disc_info->top_menu_supported,
             disc_info->num_hdmv_titles, disc_info->num_bdj_titles,
             disc_info->num_unsupported_titles);
    /* AACS */
    if (disc_info->aacs_detected) {
        msg_Dbg(p_demux, "Disc is using AACS");
        if (!disc_info->libaacs_detected)
            BLURAY_ERROR(_("This Blu-ray Disc needs a library for AACS decoding"
                      ", and your system does not have it."));
        if (!disc_info->aacs_handled) {
            if (disc_info->aacs_error_code) {
                switch (disc_info->aacs_error_code) {
                case BD_AACS_CORRUPTED_DISC:
                    BLURAY_ERROR(_("Blu-ray Disc is corrupted."));
                case BD_AACS_NO_CONFIG:
                    BLURAY_ERROR(_("Missing AACS configuration file!"));
                case BD_AACS_NO_PK:
                    BLURAY_ERROR(_("No valid processing key found in AACS config file."));
                case BD_AACS_NO_CERT:
                    BLURAY_ERROR(_("No valid host certificate found in AACS config file."));
                case BD_AACS_CERT_REVOKED:
                    BLURAY_ERROR(_("AACS Host certificate revoked."));
                case BD_AACS_MMC_FAILED:
                    BLURAY_ERROR(_("AACS MMC failed."));
                }
            }
        }
    }
    /* BD+ */
    if (disc_info->bdplus_detected) {
        msg_Dbg(p_demux, "Disc is using BD+");
        if (!disc_info->libbdplus_detected)
            BLURAY_ERROR(_("This Blu-ray Disc needs a library for BD+ decoding"
                      ", and your system does not have it."));
        if (!disc_info->bdplus_handled)
            BLURAY_ERROR(_("Your system BD+ decoding library does not work. "
                      "Missing configuration?"));
    }
    /* set player region code */
    char *psz_region = var_InheritString(p_demux, "bluray-region");
    unsigned int region = psz_region ? (psz_region[0] - 'A') : REGION_DEFAULT;
    free(psz_region);
    bd_set_player_setting(p_sys->bluray, BLURAY_PLAYER_SETTING_REGION_CODE, 1<<region);
    /* set preferred languages */
    const char *psz_code = DemuxGetLanguageCode( p_demux, "audio-language" );
    bd_set_player_setting_str(p_sys->bluray, BLURAY_PLAYER_SETTING_AUDIO_LANG, psz_code);
    psz_code = DemuxGetLanguageCode( p_demux, "sub-language" );
    bd_set_player_setting_str(p_sys->bluray, BLURAY_PLAYER_SETTING_PG_LANG,    psz_code);
    psz_code = DemuxGetLanguageCode( p_demux, "menu-language" );
    bd_set_player_setting_str(p_sys->bluray, BLURAY_PLAYER_SETTING_MENU_LANG,  psz_code);
    /* Get titles and chapters */
    p_sys->p_meta = bd_get_meta(p_sys->bluray);
    if (!p_sys->p_meta)
        msg_Warn(p_demux, "Failed to get meta info.");
    p_sys->b_menu = var_InheritBool(p_demux, "bluray-menu");
    blurayInitTitles(p_demux, disc_info->num_hdmv_titles + disc_info->num_bdj_titles + 1/*Top Menu*/ + 1/*First Play*/);
    /*
     * Initialize the event queue, so we can receive events in blurayDemux(Menu).
     */
    bd_get_event(p_sys->bluray, NULL);
    /* Registering overlay event handler */
    bd_register_overlay_proc(p_sys->bluray, p_demux, blurayOverlayProc);
    p_sys->p_input = demux_GetParentInput(p_demux);
    if (unlikely(!p_sys->p_input)) {
        msg_Err(p_demux, "Could not get parent input");
        goto error;
    }
    if (p_sys->b_menu) {
        /* Register ARGB overlay handler for BD-J */
        if (disc_info->num_bdj_titles)
            bd_register_argb_overlay_proc(p_sys->bluray, p_demux, blurayArgbOverlayProc, NULL);
        /* libbluray will start playback from "First-Title" title */
        if (bd_play(p_sys->bluray) == 0)
            BLURAY_ERROR(_("Failed to start bluray playback. Please try without menu support."));
    } else {
        /* set start title number */
        if (bluraySetTitle(p_demux, p_sys->i_longest_title) != VLC_SUCCESS) {
            msg_Err(p_demux, "Could not set the title %d", p_sys->i_longest_title);
            goto error;
        }
    }
    vlc_array_init(&p_sys->es);
    p_sys->p_out = esOutNew(p_demux);
    if (unlikely(p_sys->p_out == NULL))
        goto error;
    blurayResetParser(p_demux);
    if (!p_sys->p_parser) {
        msg_Err(p_demux, "Failed to create TS demuxer");
        goto error;
    }
    p_demux->pf_control = blurayControl;
    p_demux->pf_demux   = blurayDemux;
    return VLC_SUCCESS;
error:
    if (error_msg)
        dialog_Fatal(p_demux, _("Blu-ray error"), "%s", error_msg);
    blurayClose(object);
    return VLC_EGENERIC;
#undef BLURAY_ERROR
}

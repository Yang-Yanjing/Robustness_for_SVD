 *****************************************************************************/
static int blurayControl(demux_t *p_demux, int query, va_list args)
{
    demux_sys_t *p_sys = p_demux->p_sys;
    bool     *pb_bool;
    int64_t  *pi_64;
    switch (query) {
    case DEMUX_CAN_SEEK:
    case DEMUX_CAN_PAUSE:
    case DEMUX_CAN_CONTROL_PACE:
         pb_bool = (bool*)va_arg(args, bool *);
         *pb_bool = true;
         break;
    case DEMUX_GET_PTS_DELAY:
        pi_64 = (int64_t*)va_arg(args, int64_t *);
        *pi_64 = INT64_C(1000) * var_InheritInteger(p_demux, "disc-caching");
        break;
    case DEMUX_SET_PAUSE_STATE:
        /* Nothing to do */
        break;
    case DEMUX_SET_TITLE:
    {
        int i_title = (int)va_arg(args, int);
        if (bluraySetTitle(p_demux, i_title) != VLC_SUCCESS)
            return VLC_EGENERIC;
        break;
    }
    case DEMUX_SET_SEEKPOINT:
    {
        int i_chapter = (int)va_arg(args, int);
        bd_seek_chapter(p_sys->bluray, i_chapter);
        p_demux->info.i_update = INPUT_UPDATE_SEEKPOINT;
        break;
    }
    case DEMUX_GET_TITLE_INFO:
    {
        input_title_t ***ppp_title = (input_title_t***)va_arg(args, input_title_t***);
        int *pi_int             = (int*)va_arg(args, int*);
        int *pi_title_offset    = (int*)va_arg(args, int*);
        int *pi_chapter_offset  = (int*)va_arg(args, int*);
        /* */
        *pi_title_offset   = 0;
        *pi_chapter_offset = 0;
        /* Duplicate local title infos */
        *pi_int = p_sys->i_title;
        *ppp_title = malloc(p_sys->i_title * sizeof(input_title_t *));
        for (unsigned int i = 0; i < p_sys->i_title; i++)
            (*ppp_title)[i] = vlc_input_title_Duplicate(p_sys->pp_title[i]);
        return VLC_SUCCESS;
    }
    case DEMUX_GET_LENGTH:
    {
        int64_t *pi_length = (int64_t*)va_arg(args, int64_t *);
        *pi_length = p_demux->info.i_title < (int)p_sys->i_title ? CUR_LENGTH : 0;
        return VLC_SUCCESS;
    }
    case DEMUX_SET_TIME:
    {
        int64_t i_time = (int64_t)va_arg(args, int64_t);
        bd_seek_time(p_sys->bluray, TO_TICKS(i_time));
        return VLC_SUCCESS;
    }
    case DEMUX_GET_TIME:
    {
        int64_t *pi_time = (int64_t*)va_arg(args, int64_t *);
        *pi_time = (int64_t)FROM_TICKS(bd_tell_time(p_sys->bluray));
        return VLC_SUCCESS;
    }
    case DEMUX_GET_POSITION:
    {
        double *pf_position = (double*)va_arg(args, double *);
        *pf_position = p_demux->info.i_title < (int)p_sys->i_title && CUR_LENGTH > 0 ?
                      (double)FROM_TICKS(bd_tell_time(p_sys->bluray))/CUR_LENGTH : 0.0;
        return VLC_SUCCESS;
    }
    case DEMUX_SET_POSITION:
    {
        double f_position = (double)va_arg(args, double);
        bd_seek_time(p_sys->bluray, TO_TICKS(f_position*CUR_LENGTH));
        return VLC_SUCCESS;
    }
    case DEMUX_GET_META:
    {
        vlc_meta_t *p_meta = (vlc_meta_t *) va_arg (args, vlc_meta_t*);
        const META_DL *meta = p_sys->p_meta;
        if (meta == NULL)
            return VLC_EGENERIC;
        if (!EMPTY_STR(meta->di_name)) vlc_meta_SetTitle(p_meta, meta->di_name);
        if (!EMPTY_STR(meta->language_code)) vlc_meta_AddExtra(p_meta, "Language", meta->language_code);
        if (!EMPTY_STR(meta->filename)) vlc_meta_AddExtra(p_meta, "Filename", meta->filename);
        if (!EMPTY_STR(meta->di_alternative)) vlc_meta_AddExtra(p_meta, "Alternative", meta->di_alternative);
        // if (meta->di_set_number > 0) vlc_meta_SetTrackNum(p_meta, meta->di_set_number);
        // if (meta->di_num_sets > 0) vlc_meta_AddExtra(p_meta, "Discs numbers in Set", meta->di_num_sets);
        if (meta->thumb_count > 0 && meta->thumbnails) {
            char *psz_thumbpath;
            if (asprintf(&psz_thumbpath, "%s" DIR_SEP "BDMV" DIR_SEP "META" DIR_SEP "DL" DIR_SEP "%s",
                          p_sys->psz_bd_path, meta->thumbnails[0].path) > 0) {
                char *psz_thumburl = vlc_path2uri(psz_thumbpath, "file");
                if (unlikely(psz_thumburl == NULL)) {
                    free(psz_thumbpath);
                    return VLC_ENOMEM;
                }
                vlc_meta_SetArtURL(p_meta, psz_thumburl);
                free(psz_thumburl);
            }
            free(psz_thumbpath);
        }
        return VLC_SUCCESS;
    }
    case DEMUX_NAV_ACTIVATE:
        if (p_sys->b_popup_available && !p_sys->b_menu_open) {
            return sendKeyEvent(p_sys, BD_VK_POPUP);
        }
        return sendKeyEvent(p_sys, BD_VK_ENTER);
    case DEMUX_NAV_UP:
        return sendKeyEvent(p_sys, BD_VK_UP);
    case DEMUX_NAV_DOWN:
        return sendKeyEvent(p_sys, BD_VK_DOWN);
    case DEMUX_NAV_LEFT:
        return sendKeyEvent(p_sys, BD_VK_LEFT);
    case DEMUX_NAV_RIGHT:
        return sendKeyEvent(p_sys, BD_VK_RIGHT);
    case DEMUX_CAN_RECORD:
    case DEMUX_GET_FPS:
    case DEMUX_SET_GROUP:
    case DEMUX_HAS_UNSUPPORTED_META:
    case DEMUX_GET_ATTACHMENTS:
        return VLC_EGENERIC;
    default:
        msg_Warn(p_demux, "unimplemented query (%d) in control", query);
        return VLC_EGENERIC;
    }
    return VLC_SUCCESS;
}

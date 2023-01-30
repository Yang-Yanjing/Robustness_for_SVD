 *****************************************************************************/
static int Control( demux_t *p_demux, int i_query, va_list args )
{
    demux_sys_t *p_sys = p_demux->p_sys;
    input_title_t ***ppp_title;
    int i;
    switch( i_query )
    {
        case DEMUX_SET_POSITION:
        case DEMUX_GET_POSITION:
        case DEMUX_GET_TIME:
        case DEMUX_GET_LENGTH:
        {
            uint32_t pos, len;
            if( dvdnav_get_position( p_sys->dvdnav, &pos, &len ) !=
                  DVDNAV_STATUS_OK || len == 0 )
            {
                return VLC_EGENERIC;
            }
            switch( i_query )
            {
            case DEMUX_GET_POSITION:
                *va_arg( args, double* ) = (double)pos / (double)len;
                return VLC_SUCCESS;
            case DEMUX_SET_POSITION:
                pos = va_arg( args, double ) * len;
                if( dvdnav_sector_search( p_sys->dvdnav, pos, SEEK_SET ) ==
                      DVDNAV_STATUS_OK )
                {
                    return VLC_SUCCESS;
                }
                break;
            case DEMUX_GET_TIME:
                if( p_sys->i_pgc_length > 0 )
                {
                    *va_arg( args, int64_t * ) = p_sys->i_pgc_length*pos/len;
                    return VLC_SUCCESS;
                }
                break;
            case DEMUX_GET_LENGTH:
                if( p_sys->i_pgc_length > 0 )
                {
                    *va_arg( args, int64_t * ) = (int64_t)p_sys->i_pgc_length;
                    return VLC_SUCCESS;
                }
                break;
            }
            return VLC_EGENERIC;
        }
        /* Special for access_demux */
        case DEMUX_CAN_PAUSE:
        case DEMUX_CAN_SEEK:
        case DEMUX_CAN_CONTROL_PACE:
            /* TODO */
            *va_arg( args, bool * ) = true;
            return VLC_SUCCESS;
        case DEMUX_SET_PAUSE_STATE:
            return VLC_SUCCESS;
        case DEMUX_GET_TITLE_INFO:
            ppp_title = va_arg( args, input_title_t*** );
            *va_arg( args, int* ) = p_sys->i_title;
            *va_arg( args, int* ) = 0; /* Title offset */
            *va_arg( args, int* ) = 1; /* Chapter offset */
            /* Duplicate title infos */
            *ppp_title = malloc( p_sys->i_title * sizeof( input_title_t * ) );
            for( i = 0; i < p_sys->i_title; i++ )
            {
                (*ppp_title)[i] = vlc_input_title_Duplicate( p_sys->title[i] );
            }
            return VLC_SUCCESS;
        case DEMUX_SET_TITLE:
            i = (int)va_arg( args, int );
            if( ( i == 0 && dvdnav_menu_call( p_sys->dvdnav, DVD_MENU_Root )
                  != DVDNAV_STATUS_OK ) ||
                ( i != 0 && dvdnav_title_play( p_sys->dvdnav, i )
                  != DVDNAV_STATUS_OK ) )
            {
                msg_Warn( p_demux, "cannot set title/chapter" );
                return VLC_EGENERIC;
            }
            p_demux->info.i_update |=
                INPUT_UPDATE_TITLE | INPUT_UPDATE_SEEKPOINT;
            p_demux->info.i_title = i;
            p_demux->info.i_seekpoint = 0;
            return VLC_SUCCESS;
        case DEMUX_SET_SEEKPOINT:
            i = va_arg( args, int );
            if( p_demux->info.i_title == 0 )
            {
                static const int argtab[] = {
                    DVD_MENU_Escape,
                    DVD_MENU_Root,
                    DVD_MENU_Title,
                    DVD_MENU_Part,
                    DVD_MENU_Subpicture,
                    DVD_MENU_Audio,
                    DVD_MENU_Angle
                };
                enum { numargs = sizeof(argtab)/sizeof(int) };
                if( (unsigned)i >= numargs || DVDNAV_STATUS_OK !=
                           dvdnav_menu_call(p_sys->dvdnav,argtab[i]) )
                    return VLC_EGENERIC;
            }
            else if( dvdnav_part_play( p_sys->dvdnav, p_demux->info.i_title,
                                       i + 1 ) != DVDNAV_STATUS_OK )
            {
                msg_Warn( p_demux, "cannot set title/chapter" );
                return VLC_EGENERIC;
            }
            p_demux->info.i_update |= INPUT_UPDATE_SEEKPOINT;
            p_demux->info.i_seekpoint = i;
            return VLC_SUCCESS;
        case DEMUX_GET_PTS_DELAY:
            *va_arg( args, int64_t * ) =
                INT64_C(1000) * var_InheritInteger( p_demux, "disc-caching" );
            return VLC_SUCCESS;
        case DEMUX_GET_META:
        {
            const char *title_name = NULL;
            dvdnav_get_title_string(p_sys->dvdnav, &title_name);
            if( (NULL != title_name) && ('\0' != title_name[0]) )
            {
                vlc_meta_t *p_meta = (vlc_meta_t*)va_arg( args, vlc_meta_t* );
                vlc_meta_Set( p_meta, vlc_meta_Title, title_name );
                return VLC_SUCCESS;
            }
            return VLC_EGENERIC;
        }
        case DEMUX_NAV_ACTIVATE:
        {
            pci_t *pci = dvdnav_get_current_nav_pci( p_sys->dvdnav );
            ButtonUpdate( p_demux, true );
            dvdnav_button_activate( p_sys->dvdnav, pci );
            break;
        }
        case DEMUX_NAV_UP:
        {
            pci_t *pci = dvdnav_get_current_nav_pci( p_sys->dvdnav );
            dvdnav_upper_button_select( p_sys->dvdnav, pci );
            break;
        }
        case DEMUX_NAV_DOWN:
        {
            pci_t *pci = dvdnav_get_current_nav_pci( p_sys->dvdnav );
            dvdnav_lower_button_select( p_sys->dvdnav, pci );
            break;
        }
        case DEMUX_NAV_LEFT:
        {
            pci_t *pci = dvdnav_get_current_nav_pci( p_sys->dvdnav );
            dvdnav_left_button_select( p_sys->dvdnav, pci );
            break;
        }
        case DEMUX_NAV_RIGHT:
        {
            pci_t *pci = dvdnav_get_current_nav_pci( p_sys->dvdnav );
            dvdnav_right_button_select( p_sys->dvdnav, pci );
            break;
        }
        /* TODO implement others */
        default:
            return VLC_EGENERIC;
    }
    return VLC_SUCCESS;
}

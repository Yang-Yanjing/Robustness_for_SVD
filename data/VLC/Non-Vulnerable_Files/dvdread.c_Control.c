 *****************************************************************************/
static int Control( demux_t *p_demux, int i_query, va_list args )
{
    demux_sys_t *p_sys = p_demux->p_sys;
    double f, *pf;
    bool *pb;
    int64_t *pi64;
    input_title_t ***ppp_title;
    int *pi_int;
    int i;
    switch( i_query )
    {
        case DEMUX_GET_POSITION:
        {
            pf = (double*) va_arg( args, double* );
            if( p_sys->i_title_blocks > 0 )
                *pf = (double)p_sys->i_title_offset / p_sys->i_title_blocks;
            else
                *pf = 0.0;
            return VLC_SUCCESS;
        }
        case DEMUX_SET_POSITION:
        {
            f = (double)va_arg( args, double );
            DvdReadSeek( p_demux, f * p_sys->i_title_blocks );
            return VLC_SUCCESS;
        }
        case DEMUX_GET_TIME:
            pi64 = (int64_t*)va_arg( args, int64_t * );
            if( p_demux->info.i_title >= 0 && p_demux->info.i_title < p_sys->i_titles )
            {
                *pi64 = (int64_t) dvdtime_to_time( &p_sys->p_cur_pgc->playback_time, 0 ) /
                        p_sys->i_title_blocks * p_sys->i_title_offset;
                return VLC_SUCCESS;
            }
            *pi64 = 0;
            return VLC_EGENERIC;
        case DEMUX_GET_LENGTH:
            pi64 = (int64_t*)va_arg( args, int64_t * );
            if( p_demux->info.i_title >= 0 && p_demux->info.i_title < p_sys->i_titles )
            {
                *pi64 = (int64_t)dvdtime_to_time( &p_sys->p_cur_pgc->playback_time, 0 );
                return VLC_SUCCESS;
            }
            *pi64 = 0;
            return VLC_EGENERIC;
        /* Special for access_demux */
        case DEMUX_CAN_PAUSE:
        case DEMUX_CAN_SEEK:
        case DEMUX_CAN_CONTROL_PACE:
            /* TODO */
            pb = (bool*)va_arg( args, bool * );
            *pb = true;
            return VLC_SUCCESS;
        case DEMUX_SET_PAUSE_STATE:
            return VLC_SUCCESS;
        case DEMUX_GET_TITLE_INFO:
            ppp_title = (input_title_t***)va_arg( args, input_title_t*** );
            pi_int    = (int*)va_arg( args, int* );
            *((int*)va_arg( args, int* )) = 1; /* Title offset */
            *((int*)va_arg( args, int* )) = 1; /* Chapter offset */
            /* Duplicate title infos */
            *pi_int = p_sys->i_titles;
            *ppp_title = malloc( p_sys->i_titles * sizeof(input_title_t *) );
            for( i = 0; i < p_sys->i_titles; i++ )
            {
                (*ppp_title)[i] = vlc_input_title_Duplicate(p_sys->titles[i]);
            }
            return VLC_SUCCESS;
        case DEMUX_SET_TITLE:
            i = (int)va_arg( args, int );
            if( DvdReadSetArea( p_demux, i, 0, -1 ) != VLC_SUCCESS )
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
            i = (int)va_arg( args, int );
            if( DvdReadSetArea( p_demux, -1, i, -1 ) != VLC_SUCCESS )
            {
                msg_Warn( p_demux, "cannot set title/chapter" );
                return VLC_EGENERIC;
            }
            p_demux->info.i_update |= INPUT_UPDATE_SEEKPOINT;
            p_demux->info.i_seekpoint = i;
            return VLC_SUCCESS;
        case DEMUX_GET_PTS_DELAY:
            pi64 = (int64_t*)va_arg( args, int64_t * );
            *pi64 =
                INT64_C(1000) * var_InheritInteger( p_demux, "disc-caching" );
            return VLC_SUCCESS;
        /* TODO implement others */
        default:
            return VLC_EGENERIC;
    }
}

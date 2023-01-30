 *****************************************************************************/
static int Control( demux_t *p_demux, int i_query, va_list args )
{
    demux_sys_t *p_sys = p_demux->p_sys;
    switch( i_query )
    {
    case DEMUX_GET_TIME:
    {
        int64_t *pi_time = (int64_t*)va_arg( args, int64_t * );
        *pi_time = GetTime( p_demux );
        return VLC_SUCCESS;;
    }
    case DEMUX_GET_POSITION:
    {
        double *pf_position = (double*)va_arg( args, double * );
        *pf_position = GetPosition( p_demux );
        return VLC_SUCCESS;
    }
    case DEMUX_SET_TIME:
    {
        int64_t i_time = (int64_t)va_arg( args, int64_t );
        return SetTime( p_demux, i_time );
    }
    case DEMUX_SET_POSITION:
    {
        double f_position = (double)va_arg( args, double );
        return SetPosition( p_demux, f_position );
    }
    case DEMUX_GET_LENGTH:
    {
        int64_t *pi_length = (int64_t*)va_arg( args, int64_t * );
        *pi_length = p_sys->pp_title[p_demux->info.i_title]->i_length;
        return VLC_SUCCESS;
    }
    /* Special for access_demux */
    case DEMUX_CAN_PAUSE:
    case DEMUX_CAN_SEEK:
    case DEMUX_CAN_CONTROL_PACE:
    {
        bool *pb_bool = (bool*)va_arg( args, bool * );
        *pb_bool = true;
        return VLC_SUCCESS;
    }
    case DEMUX_SET_PAUSE_STATE:
        return VLC_SUCCESS;
    case DEMUX_GET_TITLE_INFO:
    {
        input_title_t ***ppp_title = (input_title_t***)va_arg( args, input_title_t*** );
        int *pi_int    = (int*)va_arg( args, int* );
        int *pi_title_offset = (int*)va_arg( args, int* );
        int *pi_chapter_offset = (int*)va_arg( args, int* );
        /* */
        *pi_title_offset = 0;
        *pi_chapter_offset = 0;
        /* Duplicate title infos */
        *pi_int = p_sys->i_title;
        *ppp_title = calloc( p_sys->i_title, sizeof(input_title_t *) );
        for( int i = 0; i < p_sys->i_title; i++ )
            (*ppp_title)[i] = vlc_input_title_Duplicate( p_sys->pp_title[i] );
        return VLC_SUCCESS;
    }
    case DEMUX_SET_TITLE:
    {
        int i_title = (int)va_arg( args, int );
        if( SetTitle( p_demux, i_title ) )
            return VLC_EGENERIC;
        return VLC_SUCCESS;
    }
    case DEMUX_SET_SEEKPOINT:
    {
        int i_chapter = (int)va_arg( args, int );
        if( SetChapter( p_demux, i_chapter ) )
            return VLC_EGENERIC;
        return VLC_SUCCESS;
    }
    case DEMUX_GET_PTS_DELAY:
    {
        int64_t *pi_delay = (int64_t*)va_arg( args, int64_t * );
        *pi_delay =
            INT64_C(1000) * var_InheritInteger( p_demux, "disc-caching" );
        return VLC_SUCCESS;
    }
    case DEMUX_GET_META:
    default:
        return VLC_EGENERIC;
    }
}

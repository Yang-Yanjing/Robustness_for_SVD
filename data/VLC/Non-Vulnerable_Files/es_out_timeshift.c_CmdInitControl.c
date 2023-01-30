}
static int CmdInitControl( ts_cmd_t *p_cmd, int i_query, va_list args, bool b_copy )
{
    p_cmd->i_type = C_CONTROL;
    p_cmd->i_date = mdate();
    p_cmd->u.control.i_query = i_query;
    switch( i_query )
    {
    /* Pass-through control */
    case ES_OUT_SET_MODE:    /* arg1= int                            */
    case ES_OUT_SET_GROUP:   /* arg1= int                            */
    case ES_OUT_DEL_GROUP:   /* arg1=int i_group */
        p_cmd->u.control.u.i_int = (int)va_arg( args, int );
        break;
    case ES_OUT_SET_PCR:                /* arg1=int64_t i_pcr(microsecond!) (using default group 0)*/
    case ES_OUT_SET_NEXT_DISPLAY_TIME:  /* arg1=int64_t i_pts(microsecond) */
        p_cmd->u.control.u.i_i64 = (int64_t)va_arg( args, int64_t );
        break;
    case ES_OUT_SET_GROUP_PCR:          /* arg1= int i_group, arg2=int64_t i_pcr(microsecond!)*/
        p_cmd->u.control.u.int_i64.i_int = (int)va_arg( args, int );
        p_cmd->u.control.u.int_i64.i_i64 = (int64_t)va_arg( args, int64_t );
        break;
    case ES_OUT_SET_ES_SCRAMBLED_STATE:
        p_cmd->u.control.u.es_bool.p_es = (es_out_id_t*)va_arg( args, es_out_id_t * );
        p_cmd->u.control.u.es_bool.b_bool = (bool)va_arg( args, int );
        break;
    case ES_OUT_RESET_PCR:           /* no arg */
    case ES_OUT_SET_EOS:
        break;
    case ES_OUT_SET_META:        /* arg1=const vlc_meta_t* */
    case ES_OUT_SET_GROUP_META:  /* arg1=int i_group arg2=const vlc_meta_t* */
    {
        if( i_query == ES_OUT_SET_GROUP_META )
            p_cmd->u.control.u.int_meta.i_int = (int)va_arg( args, int );
        const vlc_meta_t *p_meta = va_arg( args, const vlc_meta_t * );
        if( b_copy )
        {
            p_cmd->u.control.u.int_meta.p_meta = vlc_meta_New();
            if( !p_cmd->u.control.u.int_meta.p_meta )
                return VLC_EGENERIC;
            vlc_meta_Merge( p_cmd->u.control.u.int_meta.p_meta, p_meta );
        }
        else
        {
            /* The cast is only needed to avoid warning */
            p_cmd->u.control.u.int_meta.p_meta = (vlc_meta_t*)p_meta;
        }
        break;
    }
    case ES_OUT_SET_GROUP_EPG:   /* arg1=int i_group arg2=const vlc_epg_t* */
    {
        p_cmd->u.control.u.int_epg.i_int = (int)va_arg( args, int );
        const vlc_epg_t *p_epg = va_arg( args, const vlc_epg_t * );
        if( b_copy )
        {
            p_cmd->u.control.u.int_epg.p_epg = vlc_epg_New( p_epg->psz_name );
            if( !p_cmd->u.control.u.int_epg.p_epg )
                return VLC_EGENERIC;
            for( int i = 0; i < p_epg->i_event; i++ )
            {
                vlc_epg_event_t *p_evt = p_epg->pp_event[i];
                vlc_epg_AddEvent( p_cmd->u.control.u.int_epg.p_epg,
                                  p_evt->i_start, p_evt->i_duration,
                                  p_evt->psz_name,
                                  p_evt->psz_short_description,
                                  p_evt->psz_description, 0 );
            }
            vlc_epg_SetCurrent( p_cmd->u.control.u.int_epg.p_epg,
                                p_epg->p_current ? p_epg->p_current->i_start : -1 );
        }
        else
        {
            /* The cast is only needed to avoid warning */
            p_cmd->u.control.u.int_epg.p_epg = (vlc_epg_t*)p_epg;
        }
        break;
    }
    /* Modified control */
    case ES_OUT_SET_ES:      /* arg1= es_out_id_t*                   */
    case ES_OUT_RESTART_ES:  /* arg1= es_out_id_t*                   */
    case ES_OUT_SET_ES_DEFAULT: /* arg1= es_out_id_t*                */
        p_cmd->u.control.u.p_es = (es_out_id_t*)va_arg( args, es_out_id_t * );
        break;
    case ES_OUT_SET_ES_STATE:/* arg1= es_out_id_t* arg2=bool   */
        p_cmd->u.control.u.es_bool.p_es = (es_out_id_t*)va_arg( args, es_out_id_t * );
        p_cmd->u.control.u.es_bool.b_bool = (bool)va_arg( args, int );
        break;
    case ES_OUT_SET_ES_FMT:     /* arg1= es_out_id_t* arg2=es_format_t* */
    {
        p_cmd->u.control.u.es_fmt.p_es = (es_out_id_t*)va_arg( args, es_out_id_t * );
        es_format_t *p_fmt = (es_format_t*)va_arg( args, es_format_t * );
        if( b_copy )
        {
            p_cmd->u.control.u.es_fmt.p_fmt = malloc( sizeof(*p_fmt) );
            if( !p_cmd->u.control.u.es_fmt.p_fmt )
                return VLC_EGENERIC;
            es_format_Copy( p_cmd->u.control.u.es_fmt.p_fmt, p_fmt );
        }
        else
        {
            p_cmd->u.control.u.es_fmt.p_fmt = p_fmt;
        }
        break;
    }
    case ES_OUT_SET_TIMES:
    {
        double f_position = (double)va_arg( args, double );
        mtime_t i_time = (mtime_t)va_arg( args, mtime_t );
        mtime_t i_length = (mtime_t)va_arg( args, mtime_t );
        p_cmd->u.control.u.times.f_position = f_position;
        p_cmd->u.control.u.times.i_time = i_time;
        p_cmd->u.control.u.times.i_length = i_length;
        break;
    }
    case ES_OUT_SET_JITTER:
    {
        mtime_t i_pts_delay = (mtime_t)va_arg( args, mtime_t );
        mtime_t i_pts_jitter = (mtime_t)va_arg( args, mtime_t );
        int     i_cr_average = (int)va_arg( args, int );
        p_cmd->u.control.u.jitter.i_pts_delay = i_pts_delay;
        p_cmd->u.control.u.jitter.i_pts_jitter = i_pts_jitter;
        p_cmd->u.control.u.jitter.i_cr_average = i_cr_average;
        break;
    }
    default:
        assert(0);
        return VLC_EGENERIC;
    }
    return VLC_SUCCESS;
}

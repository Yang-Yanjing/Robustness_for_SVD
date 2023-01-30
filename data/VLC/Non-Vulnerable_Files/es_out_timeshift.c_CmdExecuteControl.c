}
static int CmdExecuteControl( es_out_t *p_out, ts_cmd_t *p_cmd )
{
    const int i_query = p_cmd->u.control.i_query;
    switch( i_query )
    {
    /* Pass-through control */
    case ES_OUT_SET_MODE:    /* arg1= int                            */
    case ES_OUT_SET_GROUP:   /* arg1= int                            */
    case ES_OUT_DEL_GROUP:   /* arg1=int i_group */
        return es_out_Control( p_out, i_query, p_cmd->u.control.u.i_int );
    case ES_OUT_SET_PCR:                /* arg1=int64_t i_pcr(microsecond!) (using default group 0)*/
    case ES_OUT_SET_NEXT_DISPLAY_TIME:  /* arg1=int64_t i_pts(microsecond) */
        return es_out_Control( p_out, i_query, p_cmd->u.control.u.i_i64 );
    case ES_OUT_SET_GROUP_PCR:          /* arg1= int i_group, arg2=int64_t i_pcr(microsecond!)*/
        return es_out_Control( p_out, i_query, p_cmd->u.control.u.int_i64.i_int,
                                               p_cmd->u.control.u.int_i64.i_i64 );
    case ES_OUT_RESET_PCR:           /* no arg */
    case ES_OUT_SET_EOS:
        return es_out_Control( p_out, i_query );
    case ES_OUT_SET_GROUP_META:  /* arg1=int i_group arg2=const vlc_meta_t* */
        return es_out_Control( p_out, i_query, p_cmd->u.control.u.int_meta.i_int,
                                               p_cmd->u.control.u.int_meta.p_meta );
    case ES_OUT_SET_GROUP_EPG:   /* arg1=int i_group arg2=const vlc_epg_t* */
        return es_out_Control( p_out, i_query, p_cmd->u.control.u.int_epg.i_int,
                                               p_cmd->u.control.u.int_epg.p_epg );
    case ES_OUT_SET_ES_SCRAMBLED_STATE: /* arg1=int es_out_id_t* arg2=bool */
        return es_out_Control( p_out, i_query, p_cmd->u.control.u.es_bool.p_es->p_es,
                                               p_cmd->u.control.u.es_bool.b_bool );
    case ES_OUT_SET_META:  /* arg1=const vlc_meta_t* */
        return es_out_Control( p_out, i_query, p_cmd->u.control.u.int_meta.p_meta );
    /* Modified control */
    case ES_OUT_SET_ES:      /* arg1= es_out_id_t*                   */
    case ES_OUT_RESTART_ES:  /* arg1= es_out_id_t*                   */
    case ES_OUT_SET_ES_DEFAULT: /* arg1= es_out_id_t*                */
        return es_out_Control( p_out, i_query, p_cmd->u.control.u.p_es->p_es );
    case ES_OUT_SET_ES_STATE:/* arg1= es_out_id_t* arg2=bool   */
        return es_out_Control( p_out, i_query, p_cmd->u.control.u.es_bool.p_es->p_es,
                                               p_cmd->u.control.u.es_bool.b_bool );
    case ES_OUT_SET_ES_FMT:     /* arg1= es_out_id_t* arg2=es_format_t* */
        return es_out_Control( p_out, i_query, p_cmd->u.control.u.es_fmt.p_es->p_es,
                                               p_cmd->u.control.u.es_fmt.p_fmt );
    case ES_OUT_SET_TIMES:
        return es_out_Control( p_out, i_query, p_cmd->u.control.u.times.f_position,
                                               p_cmd->u.control.u.times.i_time,
                                               p_cmd->u.control.u.times.i_length );
    case ES_OUT_SET_JITTER:
        return es_out_Control( p_out, i_query, p_cmd->u.control.u.jitter.i_pts_delay,
                                               p_cmd->u.control.u.jitter.i_pts_jitter,
                                               p_cmd->u.control.u.jitter.i_cr_average );
    default:
        assert(0);
        return VLC_EGENERIC;
    }
}

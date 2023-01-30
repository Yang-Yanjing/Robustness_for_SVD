
static int EsOutControlLocked( es_out_t *out, int i_query, va_list args )
{
    es_out_sys_t *p_sys = out->p_sys;
    switch( i_query )
    {
    case ES_OUT_SET_ES_STATE:
    {
        es_out_id_t *es = va_arg( args, es_out_id_t * );
        bool b = va_arg( args, int );
        if( b && !EsIsSelected( es ) )
        {
            EsSelect( out, es );
            return EsIsSelected( es ) ? VLC_SUCCESS : VLC_EGENERIC;
        }
        else if( !b && EsIsSelected( es ) )
        {
            EsUnselect( out, es, es->p_pgrm == p_sys->p_pgrm );
            return VLC_SUCCESS;
        }
        return VLC_SUCCESS;
    }
    case ES_OUT_GET_ES_STATE:
    {
        es_out_id_t *es = va_arg( args, es_out_id_t * );
        bool *pb = va_arg( args, bool * );
        *pb = EsIsSelected( es );
        return VLC_SUCCESS;
    }
    case ES_OUT_GET_GROUP_FORCED:
    {
        int *pi_group = va_arg( args, int * );
        *pi_group = p_sys->i_group_id;
        return VLC_SUCCESS;
    }
    case ES_OUT_SET_MODE:
    {
        const int i_mode = va_arg( args, int );
        assert( i_mode == ES_OUT_MODE_NONE || i_mode == ES_OUT_MODE_ALL ||
                i_mode == ES_OUT_MODE_AUTO || i_mode == ES_OUT_MODE_PARTIAL ||
                i_mode == ES_OUT_MODE_END );
        if( i_mode != ES_OUT_MODE_NONE && !p_sys->b_active && p_sys->i_es > 0 )
        {
            /* XXX Terminate vout if there are tracks but no video one.
             * This one is not mandatory but is he earliest place where it
             * can be done */
            int i;
            for( i = 0; i < p_sys->i_es; i++ )
            {
                es_out_id_t *p_es = p_sys->es[i];
                if( p_es->fmt.i_cat == VIDEO_ES )
                    break;
            }
            if( i >= p_sys->i_es )
                input_resource_TerminateVout( p_sys->p_input->p->p_resource );
        }
        p_sys->b_active = i_mode != ES_OUT_MODE_NONE;
        p_sys->i_mode = i_mode;
        /* Reapply policy mode */
        for( int i = 0; i < p_sys->i_es; i++ )
        {
            if( EsIsSelected( p_sys->es[i] ) )
                EsUnselect( out, p_sys->es[i],
                            p_sys->es[i]->p_pgrm == p_sys->p_pgrm );
        }
        for( int i = 0; i < p_sys->i_es; i++ )
            EsOutSelect( out, p_sys->es[i], false );
        if( i_mode == ES_OUT_MODE_END )
            EsOutTerminate( out );
        return VLC_SUCCESS;
    }
    case ES_OUT_SET_ES:
    case ES_OUT_RESTART_ES:
    {
        es_out_id_t *es = va_arg( args, es_out_id_t * );
        int i_cat;
        if( es == NULL )
            i_cat = UNKNOWN_ES;
        else if( es == (es_out_id_t*)((uint8_t*)NULL+AUDIO_ES) )
            i_cat = AUDIO_ES;
        else if( es == (es_out_id_t*)((uint8_t*)NULL+VIDEO_ES) )
            i_cat = VIDEO_ES;
        else if( es == (es_out_id_t*)((uint8_t*)NULL+SPU_ES) )
            i_cat = SPU_ES;
        else
            i_cat = -1;
        for( int i = 0; i < p_sys->i_es; i++ )
        {
            if( i_cat == -1 )
            {
                if( es == p_sys->es[i] )
                {
                    EsOutSelect( out, es, true );
                    break;
                }
            }
            else
            {
                if( i_cat == UNKNOWN_ES || p_sys->es[i]->fmt.i_cat == i_cat )
                {
                    if( EsIsSelected( p_sys->es[i] ) )
                    {
                        if( i_query == ES_OUT_RESTART_ES )
                        {
                            if( p_sys->es[i]->p_dec )
                            {
                                EsDestroyDecoder( out, p_sys->es[i] );
                                EsCreateDecoder( out, p_sys->es[i] );
                            }
                        }
                        else
                        {
                            EsUnselect( out, p_sys->es[i],
                                        p_sys->es[i]->p_pgrm == p_sys->p_pgrm );
                        }
                    }
                }
            }
        }
        return VLC_SUCCESS;
    }
    case ES_OUT_SET_ES_DEFAULT:
    {
        es_out_id_t *es = va_arg( args, es_out_id_t * );
        if( es == NULL )
        {
            /*p_sys->i_default_video_id = -1;*/
            /*p_sys->i_default_audio_id = -1;*/
            p_sys->i_default_sub_id = -1;
        }
        else if( es == (es_out_id_t*)((uint8_t*)NULL+AUDIO_ES) )
        {
            /*p_sys->i_default_video_id = -1;*/
        }
        else if( es == (es_out_id_t*)((uint8_t*)NULL+VIDEO_ES) )
        {
            /*p_sys->i_default_audio_id = -1;*/
        }
        else if( es == (es_out_id_t*)((uint8_t*)NULL+SPU_ES) )
        {
            p_sys->i_default_sub_id = -1;
        }
        else
        {
            /*if( es->fmt.i_cat == VIDEO_ES )
                p_sys->i_default_video_id = es->i_id;
            else
            if( es->fmt.i_cat == AUDIO_ES )
                p_sys->i_default_audio_id = es->i_id;
            else*/
            if( es->fmt.i_cat == SPU_ES )
                p_sys->i_default_sub_id = es->i_id;
        }
        return VLC_SUCCESS;
    }
    case ES_OUT_SET_PCR:
    case ES_OUT_SET_GROUP_PCR:
    {
        es_out_pgrm_t *p_pgrm = NULL;
        int            i_group = 0;
        int64_t        i_pcr;
        /* Search program */
        if( i_query == ES_OUT_SET_PCR )
        {
            p_pgrm = p_sys->p_pgrm;
            if( !p_pgrm )
                p_pgrm = EsOutProgramAdd( out, i_group );   /* Create it */
        }
        else
        {
            i_group = (int)va_arg( args, int );
            p_pgrm = EsOutProgramFind( out, i_group );
        }
        if( !p_pgrm )
            return VLC_EGENERIC;
        i_pcr = (int64_t)va_arg( args, int64_t );
        if( i_pcr <= VLC_TS_INVALID )
        {
            msg_Err( p_sys->p_input, "Invalid PCR value in ES_OUT_SET_(GROUP_)PCR !" );
            return VLC_EGENERIC;
        }
        /* TODO do not use mdate() but proper stream acquisition date */
        bool b_late;
        input_clock_Update( p_pgrm->p_clock, VLC_OBJECT(p_sys->p_input),
                            &b_late,
                            p_sys->p_input->p->b_can_pace_control || p_sys->b_buffering,
                            EsOutIsExtraBufferingAllowed( out ),
                            i_pcr, mdate() );
        if( !p_sys->p_pgrm )
            return VLC_SUCCESS;
        if( p_sys->b_buffering )
        {
            /* Check buffering state on master clock update */
            EsOutDecodersStopBuffering( out, false );
        }
        else if( p_pgrm == p_sys->p_pgrm )
        {
            if( b_late && ( !p_sys->p_input->p->p_sout ||
                                 !p_sys->p_input->p->b_out_pace_control ) )
            {
                const mtime_t i_pts_delay_base = p_sys->i_pts_delay - p_sys->i_pts_jitter;
                mtime_t i_pts_delay = input_clock_GetJitter( p_pgrm->p_clock );
                /* Avoid dangerously high value */
                const mtime_t i_jitter_max = INT64_C(1000) * var_InheritInteger( p_sys->p_input, "clock-jitter" );
                if( i_pts_delay > __MIN( i_pts_delay_base + i_jitter_max, INPUT_PTS_DELAY_MAX ) )
                {
                    msg_Err( p_sys->p_input,
                             "ES_OUT_SET_(GROUP_)PCR  is called too late (jitter of %d ms ignored)",
                             (int)(i_pts_delay - i_pts_delay_base) / 1000 );
                    i_pts_delay = p_sys->i_pts_delay;
                    /* reset clock */
                    for( int i = 0; i < p_sys->i_pgrm; i++ )
                      input_clock_Reset( p_sys->pgrm[i]->p_clock );
                }
                else
                {
                    msg_Err( p_sys->p_input,
                             "ES_OUT_SET_(GROUP_)PCR  is called too late (pts_delay increased to %d ms)",
                             (int)(i_pts_delay/1000) );
                    /* Force a rebufferization when we are too late */
                    /* It is not really good, as we throw away already buffered data
                     * TODO have a mean to correctly reenter bufferization */
                    es_out_Control( out, ES_OUT_RESET_PCR );
                }
                es_out_SetJitter( out, i_pts_delay_base, i_pts_delay - i_pts_delay_base, p_sys->i_cr_average );
            }
        }
        return VLC_SUCCESS;
    }
    case ES_OUT_RESET_PCR:
        msg_Err( p_sys->p_input, "ES_OUT_RESET_PCR called" );
        EsOutChangePosition( out );
        return VLC_SUCCESS;
    case ES_OUT_SET_GROUP:
    {
        int i = va_arg( args, int );
        for( int j = 0; j < p_sys->i_pgrm; j++ )
        {
            es_out_pgrm_t *p_pgrm = p_sys->pgrm[j];
            if( p_pgrm->i_id == i )
            {
                EsOutProgramSelect( out, p_pgrm );
                return VLC_SUCCESS;
            }
        }
        return VLC_EGENERIC;
    }
    case ES_OUT_SET_ES_FMT:
    {
        /* This ain't pretty but is need by some demuxers (eg. Ogg )
         * to update the p_extra data */
        es_out_id_t *es = va_arg( args, es_out_id_t * );
        es_format_t *p_fmt = va_arg( args, es_format_t * );
        if( es == NULL )
            return VLC_EGENERIC;
        if( p_fmt->i_extra )
        {
            es->fmt.i_extra = p_fmt->i_extra;
            es->fmt.p_extra = xrealloc( es->fmt.p_extra, p_fmt->i_extra );
            memcpy( es->fmt.p_extra, p_fmt->p_extra, p_fmt->i_extra );
            if( !es->p_dec )
                return VLC_SUCCESS;
#if 1
            EsDestroyDecoder( out, es );
            EsCreateDecoder( out, es );
#else
            es->p_dec->fmt_in.i_extra = p_fmt->i_extra;
            es->p_dec->fmt_in.p_extra =
              xrealloc( es->p_dec->fmt_in.p_extra, p_fmt->i_extra );
            memcpy( es->p_dec->fmt_in.p_extra,
                    p_fmt->p_extra, p_fmt->i_extra );
#endif
        }
        return VLC_SUCCESS;
    }
    case ES_OUT_SET_ES_SCRAMBLED_STATE:
    {
        es_out_id_t *es = va_arg( args, es_out_id_t * );
        bool b_scrambled = (bool)va_arg( args, int );
        if( !es->b_scrambled != !b_scrambled )
        {
            es->b_scrambled = b_scrambled;
            EsOutProgramUpdateScrambled( out, es->p_pgrm );
        }
        return VLC_SUCCESS;
    }
    case ES_OUT_SET_NEXT_DISPLAY_TIME:
    {
        const int64_t i_date = (int64_t)va_arg( args, int64_t );
        if( i_date < 0 )
            return VLC_EGENERIC;
        p_sys->i_preroll_end = i_date;
        return VLC_SUCCESS;
    }
    case ES_OUT_SET_GROUP_META:
    {
        int i_group = (int)va_arg( args, int );
        const vlc_meta_t *p_meta = va_arg( args, const vlc_meta_t * );
        EsOutProgramMeta( out, i_group, p_meta );
        return VLC_SUCCESS;
    }
    case ES_OUT_SET_GROUP_EPG:
    {
        int i_group = (int)va_arg( args, int );
        const vlc_epg_t *p_epg = va_arg( args, const vlc_epg_t * );
        EsOutProgramEpg( out, i_group, p_epg );
        return VLC_SUCCESS;
    }
    case ES_OUT_DEL_GROUP:
    {
        int i_group = (int)va_arg( args, int );
        return EsOutProgramDel( out, i_group );
    }
    case ES_OUT_SET_META:
    {
        const vlc_meta_t *p_meta = va_arg( args, const vlc_meta_t * );
        EsOutMeta( out, p_meta );
        return VLC_SUCCESS;
    }
    case ES_OUT_GET_WAKE_UP:
    {
        mtime_t *pi_wakeup = (mtime_t*)va_arg( args, mtime_t* );
        *pi_wakeup = EsOutGetWakeup( out );
        return VLC_SUCCESS;
    }
    case ES_OUT_SET_ES_BY_ID:
    case ES_OUT_RESTART_ES_BY_ID:
    case ES_OUT_SET_ES_DEFAULT_BY_ID:
    {
        const int i_id = (int)va_arg( args, int );
        es_out_id_t *p_es = EsOutGetFromID( out, i_id );
        int i_new_query = 0;
        switch( i_query )
        {
        case ES_OUT_SET_ES_BY_ID:         i_new_query = ES_OUT_SET_ES; break;
        case ES_OUT_RESTART_ES_BY_ID:     i_new_query = ES_OUT_RESTART_ES; break;
        case ES_OUT_SET_ES_DEFAULT_BY_ID: i_new_query = ES_OUT_SET_ES_DEFAULT; break;
        default:
          assert(0);
        }
        /* TODO if the lock is made non recursive it should be changed */
        int i_ret = es_out_Control( out, i_new_query, p_es );
        /* Clean up vout after user action (in active mode only).
         * FIXME it does not work well with multiple video windows */
        if( p_sys->b_active )
            input_resource_TerminateVout( p_sys->p_input->p->p_resource );
        return i_ret;
    }
    case ES_OUT_GET_ES_OBJECTS_BY_ID:
    {
        const int i_id = va_arg( args, int );
        es_out_id_t *p_es = EsOutGetFromID( out, i_id );
        if( !p_es )
            return VLC_EGENERIC;
        vlc_object_t    **pp_decoder = va_arg( args, vlc_object_t ** );
        vout_thread_t   **pp_vout    = va_arg( args, vout_thread_t ** );
        audio_output_t **pp_aout    = va_arg( args, audio_output_t ** );
        if( p_es->p_dec )
        {
            if( pp_decoder )
                *pp_decoder = vlc_object_hold( p_es->p_dec );
            input_DecoderGetObjects( p_es->p_dec, pp_vout, pp_aout );
        }
        else
        {
            if( pp_decoder )
                *pp_decoder = NULL;
            if( pp_vout )
                *pp_vout = NULL;
            if( pp_aout )
                *pp_aout = NULL;
        }
        return VLC_SUCCESS;
    }
    case ES_OUT_GET_BUFFERING:
    {
        bool *pb = va_arg( args, bool* );
        *pb = p_sys->b_buffering;
        return VLC_SUCCESS;
    }
    case ES_OUT_GET_EMPTY:
    {
        bool *pb = va_arg( args, bool* );
        *pb = EsOutDecodersIsEmpty( out );
        return VLC_SUCCESS;
    }
    case ES_OUT_SET_DELAY:
    {
        const int i_cat = (int)va_arg( args, int );
        const mtime_t i_delay = (mtime_t)va_arg( args, mtime_t );
        EsOutSetDelay( out, i_cat, i_delay );
        return VLC_SUCCESS;
    }
    case ES_OUT_SET_RECORD_STATE:
    {
        bool b = va_arg( args, int );
        return EsOutSetRecord( out, b );
    }
    case ES_OUT_SET_PAUSE_STATE:
    {
        const bool b_source_paused = (bool)va_arg( args, int );
        const bool b_paused = (bool)va_arg( args, int );
        const mtime_t i_date = (mtime_t) va_arg( args, mtime_t );
        assert( !b_source_paused == !b_paused );
        EsOutChangePause( out, b_paused, i_date );
        return VLC_SUCCESS;
    }
    case ES_OUT_SET_RATE:
    {
        const int i_src_rate = (int)va_arg( args, int );
        const int i_rate = (int)va_arg( args, int );
        assert( i_src_rate == i_rate );
        EsOutChangeRate( out, i_rate );
        return VLC_SUCCESS;
    }
    case ES_OUT_SET_TIME:
    {
        const mtime_t i_date = (mtime_t)va_arg( args, mtime_t );
        assert( i_date == -1 );
        EsOutChangePosition( out );
        return VLC_SUCCESS;
    }
    case ES_OUT_SET_FRAME_NEXT:
        EsOutFrameNext( out );
        return VLC_SUCCESS;
    case ES_OUT_SET_TIMES:
    {
        double f_position = (double)va_arg( args, double );
        mtime_t i_time = (mtime_t)va_arg( args, mtime_t );
        mtime_t i_length = (mtime_t)va_arg( args, mtime_t );
        input_SendEventLength( p_sys->p_input, i_length );
        if( !p_sys->b_buffering )
        {
            mtime_t i_delay;
            /* Fix for buffering delay */
            if( !p_sys->p_input->p->p_sout ||
                !p_sys->p_input->p->b_out_pace_control )
                i_delay = EsOutGetBuffering( out );
            else
                i_delay = 0;
            i_time -= i_delay;
            if( i_time < 0 )
                i_time = 0;
            if( i_length > 0 )
                f_position -= (double)i_delay / i_length;
            if( f_position < 0 )
                f_position = 0;
            input_SendEventPosition( p_sys->p_input, f_position, i_time );
        }
        return VLC_SUCCESS;
    }
    case ES_OUT_SET_JITTER:
    {
        mtime_t i_pts_delay  = (mtime_t)va_arg( args, mtime_t );
        mtime_t i_pts_jitter = (mtime_t)va_arg( args, mtime_t );
        int     i_cr_average = (int)va_arg( args, int );
        bool b_change_clock =
            i_pts_delay + i_pts_jitter != p_sys->i_pts_delay ||
            i_cr_average != p_sys->i_cr_average;
        assert( i_pts_jitter >= 0 );
        p_sys->i_pts_delay  = i_pts_delay + i_pts_jitter;
        p_sys->i_pts_jitter = i_pts_jitter;
        p_sys->i_cr_average = i_cr_average;
        for( int i = 0; i < p_sys->i_pgrm && b_change_clock; i++ )
            input_clock_SetJitter( p_sys->pgrm[i]->p_clock,
                                   i_pts_delay + i_pts_jitter, i_cr_average );
        return VLC_SUCCESS;
    }
    case ES_OUT_GET_PCR_SYSTEM:
    {
        if( p_sys->b_buffering )
            return VLC_EGENERIC;
        es_out_pgrm_t *p_pgrm = p_sys->p_pgrm;
        if( !p_pgrm )
            return VLC_EGENERIC;
        mtime_t *pi_system = va_arg( args, mtime_t *);
        mtime_t *pi_delay  = va_arg( args, mtime_t *);
        input_clock_GetSystemOrigin( p_pgrm->p_clock, pi_system, pi_delay );
        return VLC_SUCCESS;
    }
    case ES_OUT_MODIFY_PCR_SYSTEM:
    {
        if( p_sys->b_buffering )
            return VLC_EGENERIC;
        es_out_pgrm_t *p_pgrm = p_sys->p_pgrm;
        if( !p_pgrm )
            return VLC_EGENERIC;
        const bool    b_absolute = va_arg( args, int );
        const mtime_t i_system   = va_arg( args, mtime_t );
        input_clock_ChangeSystemOrigin( p_pgrm->p_clock, b_absolute, i_system );
        return VLC_SUCCESS;
    }
    case ES_OUT_SET_EOS:
    {
        for (int i = 0; i < p_sys->i_es; i++) {
            es_out_id_t *id = p_sys->es[i];
            decoder_t *p_dec = id->p_dec;
            if (!p_dec)
                continue;
            block_t *p_block = block_Alloc(0);
            if( !p_block )
                break;
            p_block->i_flags |= BLOCK_FLAG_CORE_EOS;
            input_DecoderDecode(p_dec, p_block, false);
        }
        return VLC_SUCCESS;
    }
    default:
        msg_Err( p_sys->p_input, "unknown query in es_out_Control" );
        return VLC_EGENERIC;
    }
}

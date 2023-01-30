static bool Control( input_thread_t *p_input,
                     int i_type, vlc_value_t val )
{
    const mtime_t i_control_date = mdate();
    /* FIXME b_force_update is abused, it should be carefully checked */
    bool b_force_update = false;
    if( !p_input )
        return b_force_update;
    switch( i_type )
    {
        case INPUT_CONTROL_SET_DIE:
            msg_Dbg( p_input, "control: stopping input" );
            /* Mark all submodules to die */
            ObjectKillChildrens( VLC_OBJECT(p_input) );
            break;
        case INPUT_CONTROL_SET_POSITION:
        {
            double f_pos;
            if( p_input->p->b_recording )
            {
                msg_Err( p_input, "INPUT_CONTROL_SET_POSITION(_OFFSET) ignored while recording" );
                break;
            }
            f_pos = val.f_float;
            if( i_type != INPUT_CONTROL_SET_POSITION )
                f_pos += var_GetFloat( p_input, "position" );
            if( f_pos < 0.0 )
                f_pos = 0.0;
            else if( f_pos > 1.0 )
                f_pos = 1.0;
            /* Reset the decoders states and clock sync (before calling the demuxer */
            es_out_SetTime( p_input->p->p_es_out, -1 );
            if( demux_Control( p_input->p->input.p_demux, DEMUX_SET_POSITION,
                                f_pos, !p_input->p->b_fast_seek ) )
            {
                msg_Err( p_input, "INPUT_CONTROL_SET_POSITION(_OFFSET) "
                         "%2.1f%% failed", f_pos * 100 );
            }
            else
            {
                if( p_input->p->i_slave > 0 )
                    SlaveSeek( p_input );
                p_input->p->input.b_eof = false;
                b_force_update = true;
            }
            break;
        }
        case INPUT_CONTROL_SET_TIME:
        {
            int64_t i_time;
            int i_ret;
            if( p_input->p->b_recording )
            {
                msg_Err( p_input, "INPUT_CONTROL_SET_TIME(_OFFSET) ignored while recording" );
                break;
            }
            i_time = val.i_time;
            if( i_type != INPUT_CONTROL_SET_TIME )
                i_time += var_GetTime( p_input, "time" );
            if( i_time < 0 )
                i_time = 0;
            /* Reset the decoders states and clock sync (before calling the demuxer */
            es_out_SetTime( p_input->p->p_es_out, -1 );
            i_ret = demux_Control( p_input->p->input.p_demux,
                                   DEMUX_SET_TIME, i_time,
                                   !p_input->p->b_fast_seek );
            if( i_ret )
            {
                int64_t i_length;
                /* Emulate it with a SET_POS */
                if( !demux_Control( p_input->p->input.p_demux,
                                    DEMUX_GET_LENGTH, &i_length ) && i_length > 0 )
                {
                    double f_pos = (double)i_time / (double)i_length;
                    i_ret = demux_Control( p_input->p->input.p_demux,
                                            DEMUX_SET_POSITION, f_pos,
                                            !p_input->p->b_fast_seek );
                }
            }
            if( i_ret )
            {
                msg_Warn( p_input, "INPUT_CONTROL_SET_TIME(_OFFSET) %"PRId64
                         " failed or not possible", i_time );
            }
            else
            {
                if( p_input->p->i_slave > 0 )
                    SlaveSeek( p_input );
                p_input->p->input.b_eof = false;
                b_force_update = true;
            }
            break;
        }
        case INPUT_CONTROL_SET_STATE:
            if( val.i_int != PLAYING_S && val.i_int != PAUSE_S )
                msg_Err( p_input, "invalid state in INPUT_CONTROL_SET_STATE" );
            else if( p_input->p->i_state == PAUSE_S )
            {
                ControlUnpause( p_input, i_control_date );
                b_force_update = true;
            }
            else if( val.i_int == PAUSE_S && p_input->p->i_state == PLAYING_S /* &&
                     p_input->p->b_can_pause */ )
            {
                ControlPause( p_input, i_control_date );
                b_force_update = true;
            }
            else if( val.i_int == PAUSE_S && !p_input->p->b_can_pause && 0 )
            {
                b_force_update = true;
                /* Correct "state" value */
                input_ChangeState( p_input, p_input->p->i_state );
            }
            break;
        case INPUT_CONTROL_SET_RATE:
        {
            /* Get rate and direction */
            int i_rate = abs( val.i_int );
            int i_rate_sign = val.i_int < 0 ? -1 : 1;
            /* Check rate bound */
            if( i_rate < INPUT_RATE_MIN )
            {
                msg_Dbg( p_input, "cannot set rate faster" );
                i_rate = INPUT_RATE_MIN;
            }
            else if( i_rate > INPUT_RATE_MAX )
            {
                msg_Dbg( p_input, "cannot set rate slower" );
                i_rate = INPUT_RATE_MAX;
            }
            /* Apply direction */
            if( i_rate_sign < 0 )
            {
                if( p_input->p->input.b_rescale_ts )
                {
                    msg_Dbg( p_input, "cannot set negative rate" );
                    i_rate = p_input->p->i_rate;
                    assert( i_rate > 0 );
                }
                else
                {
                    i_rate *= i_rate_sign;
                }
            }
            if( i_rate != INPUT_RATE_DEFAULT &&
                ( ( !p_input->p->b_can_rate_control && !p_input->p->input.b_rescale_ts ) ||
                  ( p_input->p->p_sout && !p_input->p->b_out_pace_control ) ) )
            {
                msg_Dbg( p_input, "cannot change rate" );
                i_rate = INPUT_RATE_DEFAULT;
            }
            if( i_rate != p_input->p->i_rate &&
                !p_input->p->b_can_pace_control && p_input->p->b_can_rate_control )
            {
                demux_t *p_demux = p_input->p->input.p_demux;
                int i_ret = VLC_EGENERIC;
                if( p_demux->s == NULL )
                {
                    if( !p_input->p->input.b_rescale_ts )
                        es_out_Control( p_input->p->p_es_out, ES_OUT_RESET_PCR );
                    i_ret = demux_Control( p_input->p->input.p_demux,
                                            DEMUX_SET_RATE, &i_rate );
                }
                if( i_ret )
                {
                    msg_Warn( p_input, "ACCESS/DEMUX_SET_RATE failed" );
                    i_rate = p_input->p->i_rate;
                }
            }
            /* */
            if( i_rate != p_input->p->i_rate )
            {
                p_input->p->i_rate = i_rate;
                input_SendEventRate( p_input, i_rate );
                if( p_input->p->input.b_rescale_ts )
                {
                    const int i_rate_source = (p_input->p->b_can_pace_control || p_input->p->b_can_rate_control ) ? i_rate : INPUT_RATE_DEFAULT;
                    es_out_SetRate( p_input->p->p_es_out, i_rate_source, i_rate );
                }
                b_force_update = true;
            }
            break;
        }
        case INPUT_CONTROL_SET_PROGRAM:
            /* No need to force update, es_out does it if needed */
            es_out_Control( p_input->p->p_es_out,
                            ES_OUT_SET_GROUP, val.i_int );
            demux_Control( p_input->p->input.p_demux, DEMUX_SET_GROUP, val.i_int,
                            NULL );
            break;
        case INPUT_CONTROL_SET_ES:
            /* No need to force update, es_out does it if needed */
            es_out_Control( p_input->p->p_es_out_display,
                            ES_OUT_SET_ES_BY_ID, (int)val.i_int );
            demux_Control( p_input->p->input.p_demux, DEMUX_SET_ES, (int)val.i_int );
            break;
        case INPUT_CONTROL_RESTART_ES:
            es_out_Control( p_input->p->p_es_out_display,
                            ES_OUT_RESTART_ES_BY_ID, (int)val.i_int );
            break;
        case INPUT_CONTROL_SET_AUDIO_DELAY:
            input_SendEventAudioDelay( p_input, val.i_time );
            UpdatePtsDelay( p_input );
            break;
        case INPUT_CONTROL_SET_SPU_DELAY:
            input_SendEventSubtitleDelay( p_input, val.i_time );
            UpdatePtsDelay( p_input );
            break;
        case INPUT_CONTROL_SET_TITLE:
        case INPUT_CONTROL_SET_TITLE_NEXT:
        case INPUT_CONTROL_SET_TITLE_PREV:
        {
            if( p_input->p->b_recording )
            {
                msg_Err( p_input, "INPUT_CONTROL_SET_TITLE(*) ignored while recording" );
                break;
            }
            if( p_input->p->input.i_title <= 0 )
                break;
            int i_title = p_input->p->input.p_demux->info.i_title;
            if( i_type == INPUT_CONTROL_SET_TITLE_PREV )
                i_title--;
            else if( i_type == INPUT_CONTROL_SET_TITLE_NEXT )
                i_title++;
            else
                i_title = val.i_int;
            if( i_title < 0 || i_title >= p_input->p->input.i_title )
                break;
            es_out_SetTime( p_input->p->p_es_out, -1 );
            demux_Control( p_input->p->input.p_demux,
                           DEMUX_SET_TITLE, i_title );
            input_SendEventTitle( p_input, i_title );
            break;
        }
        case INPUT_CONTROL_SET_SEEKPOINT:
        case INPUT_CONTROL_SET_SEEKPOINT_NEXT:
        case INPUT_CONTROL_SET_SEEKPOINT_PREV:
        {
            if( p_input->p->b_recording )
            {
                msg_Err( p_input, "INPUT_CONTROL_SET_SEEKPOINT(*) ignored while recording" );
                break;
            }
            if( p_input->p->input.i_title <= 0 )
                break;
            demux_t *p_demux = p_input->p->input.p_demux;
            int i_title = p_demux->info.i_title;
            int i_seekpoint = p_demux->info.i_seekpoint;
            if( i_type == INPUT_CONTROL_SET_SEEKPOINT_PREV )
            {
                int64_t i_seekpoint_time = p_input->p->input.title[i_title]->seekpoint[i_seekpoint]->i_time_offset;
                int64_t i_input_time = var_GetTime( p_input, "time" );
                if( i_seekpoint_time >= 0 && i_input_time >= 0 )
                {
                    if( i_input_time < i_seekpoint_time + 3000000 )
                        i_seekpoint--;
                }
                else
                    i_seekpoint--;
            }
            else if( i_type == INPUT_CONTROL_SET_SEEKPOINT_NEXT )
                i_seekpoint++;
            else
                i_seekpoint = val.i_int;
            if( i_seekpoint < 0
             || i_seekpoint >= p_input->p->input.title[i_title]->i_seekpoint )
                break;
            es_out_SetTime( p_input->p->p_es_out, -1 );
            demux_Control( p_input->p->input.p_demux,
                           DEMUX_SET_SEEKPOINT, i_seekpoint );
            input_SendEventSeekpoint( p_input, i_title, i_seekpoint );
            break;
        }
        case INPUT_CONTROL_ADD_SUBTITLE:
            if( val.psz_string )
                input_SubtitleFileAdd( p_input, val.psz_string, true );
            break;
        case INPUT_CONTROL_ADD_SLAVE:
            if( val.psz_string )
            {
                const char *uri = val.psz_string;
                input_source_t *slave = InputSourceNew( p_input );
                if( slave && !InputSourceInit( p_input, slave, uri, NULL, false ) )
                {
                    int64_t i_time;
                    /* Add the slave */
                    msg_Dbg( p_input, "adding %s as slave on the fly", uri );
                    /* Set position */
                    if( demux_Control( p_input->p->input.p_demux,
                                        DEMUX_GET_TIME, &i_time ) )
                    {
                        msg_Err( p_input, "demux doesn't like DEMUX_GET_TIME" );
                        InputSourceClean( slave );
                        free( slave );
                        break;
                    }
                    if( demux_Control( slave->p_demux,
                                       DEMUX_SET_TIME, i_time, true ) )
                    {
                        msg_Err( p_input, "seek failed for new slave" );
                        InputSourceClean( slave );
                        free( slave );
                        break;
                    }
                    /* Get meta (access and demux) */
                    InputUpdateMeta( p_input, slave->p_demux );
                    TAB_APPEND( p_input->p->i_slave, p_input->p->slave, slave );
                }
                else
                {
                    free( slave );
                    msg_Warn( p_input, "failed to add %s as slave", uri );
                }
            }
            break;
        case INPUT_CONTROL_SET_RECORD_STATE:
            if( !!p_input->p->b_recording != !!val.b_bool )
            {
                if( p_input->p->input.b_can_stream_record )
                {
                    if( demux_Control( p_input->p->input.p_demux,
                                       DEMUX_SET_RECORD_STATE, val.b_bool ) )
                        val.b_bool = false;
                }
                else
                {
                    if( es_out_SetRecordState( p_input->p->p_es_out_display, val.b_bool ) )
                        val.b_bool = false;
                }
                p_input->p->b_recording = val.b_bool;
                input_SendEventRecord( p_input, val.b_bool );
                b_force_update = true;
            }
            break;
        case INPUT_CONTROL_SET_FRAME_NEXT:
            if( p_input->p->i_state == PAUSE_S )
            {
                es_out_SetFrameNext( p_input->p->p_es_out );
            }
            else if( p_input->p->i_state == PLAYING_S )
            {
                ControlPause( p_input, i_control_date );
            }
            else
            {
                msg_Err( p_input, "invalid state for frame next" );
            }
            b_force_update = true;
            break;
        case INPUT_CONTROL_SET_BOOKMARK:
        {
            mtime_t time_offset = -1;
            vlc_mutex_lock( &p_input->p->p_item->lock );
            if( val.i_int >= 0 && val.i_int < p_input->p->i_bookmark )
            {
                const seekpoint_t *p_bookmark = p_input->p->pp_bookmark[val.i_int];
                time_offset = p_bookmark->i_time_offset;
            }
            vlc_mutex_unlock( &p_input->p->p_item->lock );
            if( time_offset < 0 )
            {
                msg_Err( p_input, "invalid bookmark %"PRId64, val.i_int );
                break;
            }
            val.i_time = time_offset;
            b_force_update = Control( p_input, INPUT_CONTROL_SET_TIME, val );
            break;
        }
        case INPUT_CONTROL_NAV_ACTIVATE:
        case INPUT_CONTROL_NAV_UP:
        case INPUT_CONTROL_NAV_DOWN:
        case INPUT_CONTROL_NAV_LEFT:
        case INPUT_CONTROL_NAV_RIGHT:
            demux_Control( p_input->p->input.p_demux, i_type
                           - INPUT_CONTROL_NAV_ACTIVATE + DEMUX_NAV_ACTIVATE );
            break;
        default:
            msg_Err( p_input, "not yet implemented" );
            break;
    }
    ControlRelease( i_type, val );
    return b_force_update;
}

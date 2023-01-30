}
static void ProcessGesture( intf_thread_t *p_intf )
{
    intf_sys_t *p_sys = p_intf->p_sys;
    playlist_t *p_playlist = pl_Get( p_intf );
    /* Do something */
    /* If you modify this, please try to follow this convention:
       Start with LEFT, RIGHT for playback related commands
       and UP, DOWN, for other commands */
    switch( p_sys->i_pattern )
    {
        case LEFT:
        {
            msg_Dbg( p_intf, "Go backward in the movie!" );
            input_thread_t *p_input = playlist_CurrentInput( p_playlist );
            if( p_input == NULL )
                break;
            int it = var_InheritInteger( p_intf , "short-jump-size" );
            if( it > 0 )
                var_SetTime( p_input, "time-offset", -CLOCK_FREQ * it );
            vlc_object_release( p_input );
            break;
        }
        case RIGHT:
        {
            msg_Dbg( p_intf, "Go forward in the movie!" );
            input_thread_t *p_input = playlist_CurrentInput( p_playlist );
            if( p_input == NULL )
                break;
            int it = var_InheritInteger( p_intf , "short-jump-size" );
            if( it > 0 )
                var_SetTime( p_input, "time-offset", CLOCK_FREQ * it );
            vlc_object_release( p_input );
            break;
        }
        case GESTURE(LEFT,UP,NONE,NONE):
            msg_Dbg( p_intf, "Going slower." );
            var_TriggerCallback( p_playlist, "rate-slower" );
            break;
        case GESTURE(RIGHT,UP,NONE,NONE):
            msg_Dbg( p_intf, "Going faster." );
            var_TriggerCallback( p_playlist, "rate-faster" );
            break;
        case GESTURE(LEFT,RIGHT,NONE,NONE):
        case GESTURE(RIGHT,LEFT,NONE,NONE):
        {
            msg_Dbg( p_intf, "Play/Pause" );
            input_thread_t *p_input = playlist_CurrentInput( p_playlist );
            if( p_input == NULL )
                break;
            int i_state = var_GetInteger( p_input, "state" );
            i_state = (i_state == PLAYING_S) ? PAUSE_S : PLAYING_S;
            var_SetInteger( p_input, "state", i_state );
            vlc_object_release( p_input );
            break;
        }
        case GESTURE(LEFT,DOWN,NONE,NONE):
            playlist_Prev( p_playlist );
            break;
        case GESTURE(RIGHT,DOWN,NONE,NONE):
            playlist_Next( p_playlist );
            break;
        case UP:
            msg_Dbg(p_intf, "Louder");
            playlist_VolumeUp( p_playlist, 1, NULL );
            break;
        case DOWN:
            msg_Dbg(p_intf, "Quieter");
            playlist_VolumeDown( p_playlist, 1, NULL );
            break;
        case GESTURE(UP,DOWN,NONE,NONE):
        case GESTURE(DOWN,UP,NONE,NONE):
            msg_Dbg( p_intf, "Mute sound" );
            playlist_MuteToggle( p_playlist );
            break;
        case GESTURE(UP,RIGHT,NONE,NONE):
        {
            input_thread_t *p_input = playlist_CurrentInput( p_playlist );
            if( p_input == NULL )
                break;
            vlc_value_t list, list2;
            var_Change( p_input, "audio-es", VLC_VAR_GETCHOICES,
                        &list, &list2 );
            if( list.p_list->i_count > 1 )
            {
                int i_audio_es = var_GetInteger( p_input, "audio-es" );
                int i;
                for( i = 0; i < list.p_list->i_count; i++ )
                     if( i_audio_es == list.p_list->p_values[i].i_int )
                         break;
                /* value of audio-es was not in choices list */
                if( i == list.p_list->i_count )
                {
                    msg_Warn( p_input,
                              "invalid current audio track, selecting 0" );
                    i = 0;
                }
                else if( i == list.p_list->i_count - 1 )
                    i = 1;
                else
                    i++;
                var_SetInteger( p_input, "audio-es",
                                list.p_list->p_values[i].i_int );
            }
            var_FreeList( &list, &list2 );
            vlc_object_release( p_input );
            break;
        }
        case GESTURE(DOWN,RIGHT,NONE,NONE):
        {
            input_thread_t *p_input = playlist_CurrentInput( p_playlist );
            if( p_input == NULL )
                break;
            vlc_value_t list, list2;
            var_Change( p_input, "spu-es", VLC_VAR_GETCHOICES,
                        &list, &list2 );
            if( list.p_list->i_count > 1 )
            {
                int i_audio_es = var_GetInteger( p_input, "spu-es" );
                int i;
                for( i = 0; i < list.p_list->i_count; i++ )
                     if( i_audio_es == list.p_list->p_values[i].i_int )
                         break;
                /* value of audio-es was not in choices list */
                if( i == list.p_list->i_count )
                {
                    msg_Warn( p_input,
                              "invalid current subtitle track, selecting 0" );
                    i = 0;
                }
                else if( i == list.p_list->i_count - 1 )
                    i = 1;
                else
                    i++;
                var_SetInteger( p_input, "audio-es",
                                list.p_list->p_values[i].i_int );
            }
            var_FreeList( &list, &list2 );
            vlc_object_release( p_input );
            break;
        }
        case GESTURE(UP,LEFT,NONE,NONE):
        {
            bool val = var_ToggleBool( pl_Get( p_intf ), "fullscreen" );
            if( p_sys->p_vout )
                var_SetBool( p_sys->p_vout, "fullscreen", val );
            break;
        }
        case GESTURE(DOWN,LEFT,NONE,NONE):
            /* FIXME: Should close the vout!"*/
            libvlc_Quit( p_intf->p_libvlc );
            break;
        case GESTURE(DOWN,LEFT,UP,RIGHT):
        case GESTURE(UP,RIGHT,DOWN,LEFT):
            msg_Dbg( p_intf, "a square was drawn!" );
            break;
    }
    p_sys->i_num_gestures = 0;
    p_sys->i_pattern = 0;
}

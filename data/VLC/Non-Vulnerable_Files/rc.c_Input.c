 ********************************************************************/
static int Input( vlc_object_t *p_this, char const *psz_cmd,
                  vlc_value_t oldval, vlc_value_t newval, void *p_data )
{
    VLC_UNUSED(oldval); VLC_UNUSED(p_data);
    intf_thread_t *p_intf = (intf_thread_t*)p_this;
    input_thread_t *p_input =
        playlist_CurrentInput( p_intf->p_sys->p_playlist );
    int i_error = VLC_EGENERIC;
    if( !p_input )
        return VLC_ENOOBJ;
    int state = var_GetInteger( p_input, "state" );
    if( ( state == PAUSE_S ) &&
        ( strcmp( psz_cmd, "pause" ) != 0 ) && (strcmp( psz_cmd,"frame") != 0 ) )
    {
        msg_rc( "%s", _("Press pause to continue.") );
    }
    else
    /* Parse commands that only require an input */
    if( !strcmp( psz_cmd, "pause" ) )
    {
        playlist_Pause( p_intf->p_sys->p_playlist );
        i_error = VLC_SUCCESS;
    }
    else if( !strcmp( psz_cmd, "seek" ) )
    {
        if( strlen( newval.psz_string ) > 0 &&
            newval.psz_string[strlen( newval.psz_string ) - 1] == '%' )
        {
            float f = atof( newval.psz_string ) / 100.0;
            var_SetFloat( p_input, "position", f );
        }
        else
        {
            mtime_t t = ((int64_t)atoi( newval.psz_string )) * CLOCK_FREQ;
            var_SetTime( p_input, "time", t );
        }
        i_error = VLC_SUCCESS;
    }
    else if ( !strcmp( psz_cmd, "fastforward" ) )
    {
        if( var_GetBool( p_input, "can-rate" ) )
        {
            float f_rate = var_GetFloat( p_input, "rate" );
            f_rate = (f_rate < 0) ? -f_rate : f_rate * 2;
            var_SetFloat( p_input, "rate", f_rate );
        }
        else
        {
            var_SetInteger( p_intf->p_libvlc, "key-action", ACTIONID_JUMP_FORWARD_EXTRASHORT );
        }
        i_error = VLC_SUCCESS;
    }
    else if ( !strcmp( psz_cmd, "rewind" ) )
    {
        if( var_GetBool( p_input, "can-rewind" ) )
        {
            float f_rate = var_GetFloat( p_input, "rate" );
            f_rate = (f_rate > 0) ? -f_rate : f_rate * 2;
            var_SetFloat( p_input, "rate", f_rate );
        }
        else
        {
            var_SetInteger( p_intf->p_libvlc, "key-action", ACTIONID_JUMP_BACKWARD_EXTRASHORT );
        }
        i_error = VLC_SUCCESS;
    }
    else if ( !strcmp( psz_cmd, "faster" ) )
    {
        var_TriggerCallback( p_intf->p_sys->p_playlist, "rate-faster" );
        i_error = VLC_SUCCESS;
    }
    else if ( !strcmp( psz_cmd, "slower" ) )
    {
        var_TriggerCallback( p_intf->p_sys->p_playlist, "rate-slower" );
        i_error = VLC_SUCCESS;
    }
    else if ( !strcmp( psz_cmd, "normal" ) )
    {
        var_SetFloat( p_intf->p_sys->p_playlist, "rate", 1. );
        i_error = VLC_SUCCESS;
    }
    else if ( !strcmp( psz_cmd, "frame" ) )
    {
	var_TriggerCallback( p_input, "frame-next" );
        i_error = VLC_SUCCESS;
    }
    else if( !strcmp( psz_cmd, "chapter" ) ||
             !strcmp( psz_cmd, "chapter_n" ) ||
             !strcmp( psz_cmd, "chapter_p" ) )
    {
        if( !strcmp( psz_cmd, "chapter" ) )
        {
            if ( *newval.psz_string )
            {
                /* Set. */
                var_SetInteger( p_input, "chapter", atoi( newval.psz_string ) );
            }
            else
            {
                /* Get. */
                int i_chap = var_GetInteger( p_input, "chapter" );
                int i_chapter_count = var_CountChoices( p_input, "chapter" );
                msg_rc( "Currently playing chapter %d/%d.", i_chap,
                        i_chapter_count );
            }
        }
        else if( !strcmp( psz_cmd, "chapter_n" ) )
            var_TriggerCallback( p_input, "next-chapter" );
        else if( !strcmp( psz_cmd, "chapter_p" ) )
            var_TriggerCallback( p_input, "prev-chapter" );
        i_error = VLC_SUCCESS;
    }
    else if( !strcmp( psz_cmd, "title" ) ||
             !strcmp( psz_cmd, "title_n" ) ||
             !strcmp( psz_cmd, "title_p" ) )
    {
        if( !strcmp( psz_cmd, "title" ) )
        {
            if ( *newval.psz_string )
                /* Set. */
                var_SetInteger( p_input, "title", atoi( newval.psz_string ) );
            else
            {
                /* Get. */
                int i_title = var_GetInteger( p_input, "title" );
                int i_title_count = var_CountChoices( p_input, "title" );
                msg_rc( "Currently playing title %d/%d.", i_title,
                        i_title_count );
            }
        }
        else if( !strcmp( psz_cmd, "title_n" ) )
            var_TriggerCallback( p_input, "next-title" );
        else if( !strcmp( psz_cmd, "title_p" ) )
            var_TriggerCallback( p_input, "prev-title" );
        i_error = VLC_SUCCESS;
    }
    else if(    !strcmp( psz_cmd, "atrack" )
             || !strcmp( psz_cmd, "vtrack" )
             || !strcmp( psz_cmd, "strack" ) )
    {
        const char *psz_variable;
        vlc_value_t val_name;
        if( !strcmp( psz_cmd, "atrack" ) )
        {
            psz_variable = "audio-es";
        }
        else if( !strcmp( psz_cmd, "vtrack" ) )
        {
            psz_variable = "video-es";
        }
        else
        {
            psz_variable = "spu-es";
        }
        /* Get the descriptive name of the variable */
        var_Change( p_input, psz_variable, VLC_VAR_GETTEXT,
                     &val_name, NULL );
        if( !val_name.psz_string ) val_name.psz_string = strdup(psz_variable);
        if( newval.psz_string && *newval.psz_string )
        {
            /* set */
            i_error = var_SetInteger( p_input, psz_variable,
                                      atoi( newval.psz_string ) );
        }
        else
        {
            /* get */
            vlc_value_t val, text;
            int i, i_value;
            if ( var_Get( p_input, psz_variable, &val ) < 0 )
                goto out;
            i_value = val.i_int;
            if ( var_Change( p_input, psz_variable,
                             VLC_VAR_GETLIST, &val, &text ) < 0 )
                goto out;
            msg_rc( "+----[ %s ]", val_name.psz_string );
            for ( i = 0; i < val.p_list->i_count; i++ )
            {
                if ( i_value == val.p_list->p_values[i].i_int )
                    msg_rc( "| %"PRId64" - %s *",
                            val.p_list->p_values[i].i_int,
                            text.p_list->p_values[i].psz_string );
                else
                    msg_rc( "| %"PRId64" - %s",
                            val.p_list->p_values[i].i_int,
                            text.p_list->p_values[i].psz_string );
            }
            var_FreeList( &val, &text );
            msg_rc( "+----[ end of %s ]", val_name.psz_string );
        }
        free( val_name.psz_string );
    }
out:
    vlc_object_release( p_input );
    return i_error;
}

}
static int VideoConfig( vlc_object_t *p_this, char const *psz_cmd,
                        vlc_value_t oldval, vlc_value_t newval, void *p_data )
{
    VLC_UNUSED(oldval); VLC_UNUSED(p_data);
    intf_thread_t *p_intf = (intf_thread_t*)p_this;
    input_thread_t *p_input =
        playlist_CurrentInput( p_intf->p_sys->p_playlist );
    vout_thread_t * p_vout;
    const char * psz_variable = NULL;
    int i_error = VLC_SUCCESS;
    if( !p_input )
        return VLC_ENOOBJ;
    p_vout = input_GetVout( p_input );
    vlc_object_release( p_input );
    if( !p_vout )
        return VLC_ENOOBJ;
    if( !strcmp( psz_cmd, "vcrop" ) )
    {
        psz_variable = "crop";
    }
    else if( !strcmp( psz_cmd, "vratio" ) )
    {
        psz_variable = "aspect-ratio";
    }
    else if( !strcmp( psz_cmd, "vzoom" ) )
    {
        psz_variable = "zoom";
    }
    else if( !strcmp( psz_cmd, "snapshot" ) )
    {
        psz_variable = "video-snapshot";
    }
    else
        /* This case can't happen */
        assert( 0 );
    if( newval.psz_string && *newval.psz_string )
    {
        /* set */
        if( !strcmp( psz_variable, "zoom" ) )
        {
            vlc_value_t val;
            val.f_float = atof( newval.psz_string );
            i_error = var_Set( p_vout, psz_variable, val );
        }
        else
        {
            i_error = var_Set( p_vout, psz_variable, newval );
        }
    }
    else if( !strcmp( psz_cmd, "snapshot" ) )
    {
        var_TriggerCallback( p_vout, psz_variable );
    }
    else
    {
        /* get */
        vlc_value_t val_name;
        vlc_value_t val, text;
        int i;
        float f_value = 0.;
        char *psz_value = NULL;
        if ( var_Get( p_vout, psz_variable, &val ) < 0 )
        {
            vlc_object_release( p_vout );
            return VLC_EGENERIC;
        }
        if( !strcmp( psz_variable, "zoom" ) )
        {
            f_value = val.f_float;
        }
        else
        {
            psz_value = val.psz_string;
        }
        if ( var_Change( p_vout, psz_variable,
                         VLC_VAR_GETLIST, &val, &text ) < 0 )
        {
            vlc_object_release( p_vout );
            free( psz_value );
            return VLC_EGENERIC;
        }
        /* Get the descriptive name of the variable */
        var_Change( p_vout, psz_variable, VLC_VAR_GETTEXT,
                    &val_name, NULL );
        if( !val_name.psz_string ) val_name.psz_string = strdup(psz_variable);
        msg_rc( "+----[ %s ]", val_name.psz_string );
        if( !strcmp( psz_variable, "zoom" ) )
        {
            for ( i = 0; i < val.p_list->i_count; i++ )
            {
                if ( f_value == val.p_list->p_values[i].f_float )
                    msg_rc( "| %f - %s *", val.p_list->p_values[i].f_float,
                            text.p_list->p_values[i].psz_string );
                else
                    msg_rc( "| %f - %s", val.p_list->p_values[i].f_float,
                            text.p_list->p_values[i].psz_string );
            }
        }
        else
        {
            for ( i = 0; i < val.p_list->i_count; i++ )
            {
                if ( !strcmp( psz_value, val.p_list->p_values[i].psz_string ) )
                    msg_rc( "| %s - %s *", val.p_list->p_values[i].psz_string,
                            text.p_list->p_values[i].psz_string );
                else
                    msg_rc( "| %s - %s", val.p_list->p_values[i].psz_string,
                            text.p_list->p_values[i].psz_string );
            }
            free( psz_value );
        }
        var_FreeList( &val, &text );
        msg_rc( "+----[ end of %s ]", val_name.psz_string );
        free( val_name.psz_string );
    }
    vlc_object_release( p_vout );
    return i_error;
}

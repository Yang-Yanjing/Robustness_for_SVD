*****************************************************************************/
static int MosaicCallback( vlc_object_t *p_this, char const *psz_var,
                            vlc_value_t oldval, vlc_value_t newval,
                            void *p_data )
{
    VLC_UNUSED(oldval);
    filter_sys_t *p_sys = (filter_sys_t *) p_data;
#define VAR_IS( a ) !strcmp( psz_var, CFG_PREFIX a )
    if( VAR_IS( "alpha" ) )
    {
        vlc_mutex_lock( &p_sys->lock );
        msg_Dbg( p_this, "changing alpha from %d/255 to %d/255",
                         p_sys->i_alpha, (int)newval.i_int);
        p_sys->i_alpha = VLC_CLIP( newval.i_int, 0, 255 );
        vlc_mutex_unlock( &p_sys->lock );
    }
    else if( VAR_IS( "height" ) )
    {
        vlc_mutex_lock( &p_sys->lock );
        msg_Dbg( p_this, "changing height from %dpx to %dpx",
                          p_sys->i_height, (int)newval.i_int );
        p_sys->i_height = __MAX( newval.i_int, 0 );
        vlc_mutex_unlock( &p_sys->lock );
    }
    else if( VAR_IS( "width" ) )
    {
        vlc_mutex_lock( &p_sys->lock );
        msg_Dbg( p_this, "changing width from %dpx to %dpx",
                         p_sys->i_width, (int)newval.i_int );
        p_sys->i_width = __MAX( newval.i_int, 0 );
        vlc_mutex_unlock( &p_sys->lock );
    }
    else if( VAR_IS( "xoffset" ) )
    {
        vlc_mutex_lock( &p_sys->lock );
        msg_Dbg( p_this, "changing x offset from %dpx to %dpx",
                         p_sys->i_xoffset, (int)newval.i_int );
        p_sys->i_xoffset = __MAX( newval.i_int, 0 );
        vlc_mutex_unlock( &p_sys->lock );
    }
    else if( VAR_IS( "yoffset" ) )
    {
        vlc_mutex_lock( &p_sys->lock );
        msg_Dbg( p_this, "changing y offset from %dpx to %dpx",
                         p_sys->i_yoffset, (int)newval.i_int );
        p_sys->i_yoffset = __MAX( newval.i_int, 0 );
        vlc_mutex_unlock( &p_sys->lock );
    }
    else if( VAR_IS( "align" ) )
    {
        int i_old = 0, i_new = 0;
        vlc_mutex_lock( &p_sys->lock );
        newval.i_int = VLC_CLIP( newval.i_int, 0, 10 );
        if( newval.i_int == 3 || newval.i_int == 7 )
            newval.i_int = 5;
        while( pi_align_values[i_old] != p_sys->i_align ) i_old++;
        while( pi_align_values[i_new] != newval.i_int ) i_new++;
        msg_Dbg( p_this, "changing alignment from %d (%s) to %d (%s)",
                     p_sys->i_align, ppsz_align_descriptions[i_old],
                     (int)newval.i_int, ppsz_align_descriptions[i_new] );
        p_sys->i_align = newval.i_int;
        vlc_mutex_unlock( &p_sys->lock );
    }
    else if( VAR_IS( "borderw" ) )
    {
        vlc_mutex_lock( &p_sys->lock );
        msg_Dbg( p_this, "changing border width from %dpx to %dpx",
                         p_sys->i_borderw, (int)newval.i_int );
        p_sys->i_borderw = __MAX( newval.i_int, 0 );
        vlc_mutex_unlock( &p_sys->lock );
    }
    else if( VAR_IS( "borderh" ) )
    {
        vlc_mutex_lock( &p_sys->lock );
        msg_Dbg( p_this, "changing border height from %dpx to %dpx",
                         p_sys->i_borderh, (int)newval.i_int );
        p_sys->i_borderh = __MAX( newval.i_int, 0 );
        vlc_mutex_unlock( &p_sys->lock );
    }
    else if( VAR_IS( "position" ) )
    {
        if( newval.i_int > 2 || newval.i_int < 0 )
        {
            msg_Err( p_this,
                     "Position is either 0 (%s), 1 (%s) or 2 (%s)",
                     ppsz_pos_descriptions[0],
                     ppsz_pos_descriptions[1],
                     ppsz_pos_descriptions[2] );
        }
        else
        {
            vlc_mutex_lock( &p_sys->lock );
            msg_Dbg( p_this, "changing position method from %d (%s) to %d (%s)",
                    p_sys->i_position, ppsz_pos_descriptions[p_sys->i_position],
                     (int)newval.i_int, ppsz_pos_descriptions[newval.i_int]);
            p_sys->i_position = newval.i_int;
            vlc_mutex_unlock( &p_sys->lock );
        }
    }
    else if( VAR_IS( "rows" ) )
    {
        vlc_mutex_lock( &p_sys->lock );
        msg_Dbg( p_this, "changing number of rows from %d to %d",
                         p_sys->i_rows, (int)newval.i_int );
        p_sys->i_rows = __MAX( newval.i_int, 1 );
        vlc_mutex_unlock( &p_sys->lock );
    }
    else if( VAR_IS( "cols" ) )
    {
        vlc_mutex_lock( &p_sys->lock );
        msg_Dbg( p_this, "changing number of columns from %d to %d",
                         p_sys->i_cols, (int)newval.i_int );
        p_sys->i_cols = __MAX( newval.i_int, 1 );
        vlc_mutex_unlock( &p_sys->lock );
    }
    else if( VAR_IS( "order" ) )
    {
        char *psz_order;
        int i_index;
        vlc_mutex_lock( &p_sys->lock );
        msg_Dbg( p_this, "Changing mosaic order to %s", newval.psz_string );
        psz_order = newval.psz_string;
        while( p_sys->i_order_length-- )
        {
            free( p_sys->ppsz_order[p_sys->i_order_length] );
        }
        free( p_sys->ppsz_order );
        p_sys->ppsz_order = NULL;
        if( *psz_order )
        {
            char *psz_end = NULL;
            i_index = 0;
            do
            {
                psz_end = strchr( psz_order, ',' );
                i_index++;
                p_sys->ppsz_order = xrealloc( p_sys->ppsz_order,
                                                   i_index * sizeof(char *) );
                p_sys->ppsz_order[i_index - 1] = strndup( psz_order,
                                           psz_end - psz_order );
                psz_order = psz_end+1;
            } while( psz_end );
            p_sys->i_order_length = i_index;
        }
        vlc_mutex_unlock( &p_sys->lock );
    }
    else if( VAR_IS( "offsets" ) )
    {
        vlc_mutex_lock( &p_sys->lock );
        msg_Info( p_this, "Changing mosaic-offsets to %s", newval.psz_string );
        if( p_sys->i_offsets_length != 0 )
        {
            p_sys->i_offsets_length = 0;
            free( p_sys->pi_x_offsets );
            free( p_sys->pi_y_offsets );
            p_sys->pi_x_offsets = NULL;
            p_sys->pi_y_offsets = NULL;
        }
        mosaic_ParseSetOffsets( p_this, p_sys, newval.psz_string );
        vlc_mutex_unlock( &p_sys->lock );
    }
    else if( VAR_IS( "keep-aspect-ratio" ) )
    {
        vlc_mutex_lock( &p_sys->lock );
        if( newval.i_int )
        {
            msg_Dbg( p_this, "keeping aspect ratio" );
            p_sys->b_ar = 1;
        }
        else
        {
            msg_Dbg( p_this, "won't keep aspect ratio" );
            p_sys->b_ar = 0;
        }
        vlc_mutex_unlock( &p_sys->lock );
    }
    else if( VAR_IS( "keep-picture" ) )
    {
        vlc_mutex_lock( &p_sys->lock );
        p_sys->b_keep = newval.b_bool;
        if ( !p_sys->b_keep && !p_sys->p_image )
        {
            p_sys->p_image = image_HandlerCreate( p_this );
        }
        vlc_mutex_unlock( &p_sys->lock );
    }
    return VLC_SUCCESS;
}

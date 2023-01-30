static char *MarqueeReadFile( filter_t *, const char * );
static int MarqueeCallback( vlc_object_t *p_this, char const *psz_var,
                            vlc_value_t oldval, vlc_value_t newval,
                            void *p_data );
static const int pi_color_values[] = {
               0xf0000000, 0x00000000, 0x00808080, 0x00C0C0C0,
               0x00FFFFFF, 0x00800000, 0x00FF0000, 0x00FF00FF, 0x00FFFF00,
               0x00808000, 0x00008000, 0x00008080, 0x0000FF00, 0x00800080,
               0x00000080, 0x000000FF, 0x0000FFFF};
 **********************************************************************/
static int MarqueeCallback( vlc_object_t *p_this, char const *psz_var,
                            vlc_value_t oldval, vlc_value_t newval,
                            void *p_data )
{
    filter_sys_t *p_sys = (filter_sys_t *) p_data;
    VLC_UNUSED(oldval);
    VLC_UNUSED(p_this);
    vlc_mutex_lock( &p_sys->lock );
    if( !strcmp( psz_var, "marq-marquee" ) )
    {
        free( p_sys->format );
        p_sys->format = strdup( newval.psz_string );
    }
    else if ( !strcmp( psz_var, "marq-x" ) )
    {
        p_sys->i_xoff = newval.i_int;
    }
    else if ( !strcmp( psz_var, "marq-y" ) )
    {
        p_sys->i_yoff = newval.i_int;
    }
    else if ( !strcmp( psz_var, "marq-color" ) )
    {
        p_sys->p_style->i_font_color = newval.i_int;
    }
    else if ( !strcmp( psz_var, "marq-opacity" ) )
    {
        p_sys->p_style->i_font_alpha = newval.i_int;
    }
    else if ( !strcmp( psz_var, "marq-size" ) )
    {
        p_sys->p_style->i_font_size = newval.i_int;
    }
    else if ( !strcmp( psz_var, "marq-timeout" ) )
    {
        p_sys->i_timeout = newval.i_int;
    }
    else if ( !strcmp( psz_var, "marq-refresh" ) )
    {
        p_sys->i_refresh = newval.i_int * 1000;
    }
    else if ( !strcmp( psz_var, "marq-position" ) )
    /* willing to accept a match against marq-pos */
    {
        p_sys->i_pos = newval.i_int;
        p_sys->i_xoff = -1;       /* force to relative positioning */
    }
    free( p_sys->message );
    p_sys->message = NULL; /* force update */
    vlc_mutex_unlock( &p_sys->lock );
    return VLC_SUCCESS;
}

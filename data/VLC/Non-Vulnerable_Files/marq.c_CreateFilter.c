 *****************************************************************************/
static int CreateFilter( vlc_object_t *p_this )
{
    filter_t *p_filter = (filter_t *)p_this;
    filter_sys_t *p_sys;
    /* Allocate structure */
    p_sys = p_filter->p_sys = malloc( sizeof( filter_sys_t ) );
    if( p_sys == NULL )
        return VLC_ENOMEM;
    vlc_mutex_init( &p_sys->lock );
    p_sys->p_style = text_style_New();
    config_ChainParse( p_filter, CFG_PREFIX, ppsz_filter_options,
                       p_filter->p_cfg );
#define CREATE_VAR( stor, type, var ) \
    p_sys->stor = var_CreateGet##type##Command( p_filter, var ); \
    var_AddCallback( p_filter, var, MarqueeCallback, p_sys );
    CREATE_VAR( i_xoff, Integer, "marq-x" );
    CREATE_VAR( i_yoff, Integer, "marq-y" );
    CREATE_VAR( i_timeout,Integer, "marq-timeout" );
    p_sys->i_refresh = 1000 * var_CreateGetIntegerCommand( p_filter,
                                                           "marq-refresh" );
    var_AddCallback( p_filter, "marq-refresh", MarqueeCallback, p_sys );
    CREATE_VAR( i_pos, Integer, "marq-position" );
    CREATE_VAR( format, String, "marq-marquee" );
    p_sys->filepath = var_InheritString( p_filter, "marq-file" );
    p_sys->message = NULL;
    p_sys->p_style->i_font_alpha = var_CreateGetIntegerCommand( p_filter,
                                                            "marq-opacity" );
    var_AddCallback( p_filter, "marq-opacity", MarqueeCallback, p_sys );
    CREATE_VAR( p_style->i_font_color, Integer, "marq-color" );
    CREATE_VAR( p_style->i_font_size, Integer, "marq-size" );
    /* Misc init */
    p_filter->pf_sub_source = Filter;
    p_sys->last_time = 0;
    return VLC_SUCCESS;
}

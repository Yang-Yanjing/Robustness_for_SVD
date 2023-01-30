 *****************************************************************************/
static void DestroyFilter( vlc_object_t *p_this )
{
    filter_t *p_filter = (filter_t *)p_this;
    filter_sys_t *p_sys = p_filter->p_sys;
    /* Delete the marquee variables */
#define DEL_VAR(var) \
    var_DelCallback( p_filter, var, MarqueeCallback, p_sys ); \
    var_Destroy( p_filter, var );
    DEL_VAR( "marq-x" );
    DEL_VAR( "marq-y" );
    DEL_VAR( "marq-timeout" );
    DEL_VAR( "marq-refresh" );
    DEL_VAR( "marq-position" );
    DEL_VAR( "marq-marquee" );
    DEL_VAR( "marq-opacity" );
    DEL_VAR( "marq-color" );
    DEL_VAR( "marq-size" );
    vlc_mutex_destroy( &p_sys->lock );
    text_style_Delete( p_sys->p_style );
    free( p_sys->format );
    free( p_sys->filepath );
    free( p_sys->message );
    free( p_sys );
}

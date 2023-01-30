 *****************************************************************************/
static void DestroyFilter( vlc_object_t *p_this )
{
    filter_t *p_filter = (filter_t*)p_this;
    filter_sys_t *p_sys = p_filter->p_sys;
#define DEL_CB( name ) \
    var_DelCallback( p_filter, CFG_PREFIX #name, MosaicCallback, p_sys )
    DEL_CB( width );
    DEL_CB( height );
    DEL_CB( xoffset );
    DEL_CB( yoffset );
    DEL_CB( align );
    DEL_CB( borderw );
    DEL_CB( borderh );
    DEL_CB( rows );
    DEL_CB( cols );
    DEL_CB( alpha );
    DEL_CB( position );
    DEL_CB( delay );
    DEL_CB( keep-aspect-ratio );
    DEL_CB( order );
#undef DEL_CB
    if( !p_sys->b_keep )
    {
        image_HandlerDelete( p_sys->p_image );
    }
    if( p_sys->i_order_length )
    {
        for( int i_index = 0; i_index < p_sys->i_order_length; i_index++ )
        {
            free( p_sys->ppsz_order[i_index] );
        }
        free( p_sys->ppsz_order );
    }
    if( p_sys->i_offsets_length )
    {
        free( p_sys->pi_x_offsets );
        free( p_sys->pi_y_offsets );
        p_sys->i_offsets_length = 0;
    }
    vlc_mutex_destroy( &p_sys->lock );
    free( p_sys );
}

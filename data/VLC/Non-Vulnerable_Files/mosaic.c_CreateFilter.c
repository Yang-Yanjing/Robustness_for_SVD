 *****************************************************************************/
static int CreateFilter( vlc_object_t *p_this )
{
    filter_t *p_filter = (filter_t *)p_this;
    filter_sys_t *p_sys;
    char *psz_order, *_psz_order;
    char *psz_offsets;
    int i_index;
    int i_command;
    /* Allocate structure */
    p_sys = p_filter->p_sys = malloc( sizeof( filter_sys_t ) );
    if( p_sys == NULL )
        return VLC_ENOMEM;
    p_filter->pf_sub_source = Filter;
    vlc_mutex_init( &p_sys->lock );
    vlc_mutex_lock( &p_sys->lock );
    config_ChainParse( p_filter, CFG_PREFIX, ppsz_filter_options,
                       p_filter->p_cfg );
#define GET_VAR( name, min, max )                                           \
    i_command = var_CreateGetIntegerCommand( p_filter, CFG_PREFIX #name );  \
    p_sys->i_##name = VLC_CLIP( i_command, min, max );                \
    var_AddCallback( p_filter, CFG_PREFIX #name, MosaicCallback, p_sys );
    GET_VAR( width, 0, INT_MAX );
    GET_VAR( height, 0, INT_MAX );
    GET_VAR( xoffset, 0, INT_MAX );
    GET_VAR( yoffset, 0, INT_MAX );
    GET_VAR( align, 0, 10 );
    if( p_sys->i_align == 3 || p_sys->i_align == 7 )
        p_sys->i_align = 5;
    GET_VAR( borderw, 0, INT_MAX );
    GET_VAR( borderh, 0, INT_MAX );
    GET_VAR( rows, 1, INT_MAX );
    GET_VAR( cols, 1, INT_MAX );
    GET_VAR( alpha, 0, 255 );
    GET_VAR( position, 0, 2 );
    GET_VAR( delay, 100, INT_MAX );
#undef GET_VAR
    p_sys->i_delay *= 1000;
    p_sys->b_ar = var_CreateGetBoolCommand( p_filter,
                                            CFG_PREFIX "keep-aspect-ratio" );
    var_AddCallback( p_filter, CFG_PREFIX "keep-aspect-ratio", MosaicCallback,
                     p_sys );
    p_sys->b_keep = var_CreateGetBoolCommand( p_filter,
                                              CFG_PREFIX "keep-picture" );
    if ( !p_sys->b_keep )
    {
        p_sys->p_image = image_HandlerCreate( p_filter );
    }
    p_sys->i_order_length = 0;
    p_sys->ppsz_order = NULL;
    psz_order = var_CreateGetStringCommand( p_filter, CFG_PREFIX "order" );
    _psz_order = psz_order;
    var_AddCallback( p_filter, CFG_PREFIX "order", MosaicCallback, p_sys );
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
    free( _psz_order );
    /* Manage specific offsets for substreams */
    psz_offsets = var_CreateGetStringCommand( p_filter, CFG_PREFIX "offsets" );
    p_sys->i_offsets_length = 0;
    p_sys->pi_x_offsets = NULL;
    p_sys->pi_y_offsets = NULL;
    mosaic_ParseSetOffsets( p_filter, p_sys, psz_offsets );
    free( psz_offsets );
    var_AddCallback( p_filter, CFG_PREFIX "offsets", MosaicCallback, p_sys );
    vlc_mutex_unlock( &p_sys->lock );
    return VLC_SUCCESS;
}

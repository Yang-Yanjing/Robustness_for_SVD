 *****************************************************************************/
static int Create( vlc_object_t *p_this )
{
    filter_t *p_filter = (filter_t *)p_this;
    filter_sys_t *p_sys;
    /* Allocate structure */
    p_filter->p_sys = malloc( sizeof( filter_sys_t ) );
    if( p_filter->p_sys == NULL )
        return VLC_ENOMEM;
    p_sys = p_filter->p_sys;
    BufferInit( &p_sys->input );
    BufferInit( &p_sys->output );
    QueueInit( &p_sys->atomic );
    QueueInit( &p_sys->pending );
    QueueInit( &p_sys->processed );
    do_ListInit( &p_sys->overlays );
    p_sys->i_inputfd = -1;
    p_sys->i_outputfd = -1;
    p_sys->b_updated = true;
    p_sys->b_atomic = false;
    vlc_mutex_init( &p_sys->lock );
    p_filter->pf_sub_source = Filter;
    config_ChainParse( p_filter, "overlay-", ppsz_filter_options,
                       p_filter->p_cfg );
    p_sys->psz_inputfile = var_CreateGetStringCommand( p_filter,
                                                       "overlay-input" );
    p_sys->psz_outputfile = var_CreateGetStringCommand( p_filter,
                                                        "overlay-output" );
    var_AddCallback( p_filter, "overlay-input", AdjustCallback, p_sys );
    var_AddCallback( p_filter, "overlay-output", AdjustCallback, p_sys );
    RegisterCommand( p_filter );
    return VLC_SUCCESS;
}

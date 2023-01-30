 *****************************************************************************/
static void Destroy( vlc_object_t *p_this )
{
    filter_t *p_filter = (filter_t *)p_this;
    filter_sys_t *p_sys = p_filter->p_sys;
    BufferDestroy( &p_sys->input );
    BufferDestroy( &p_sys->output );
    QueueDestroy( &p_sys->atomic );
    QueueDestroy( &p_sys->pending );
    QueueDestroy( &p_sys->processed );
    do_ListDestroy( &p_sys->overlays );
    UnregisterCommand( p_filter );
    var_DelCallback( p_filter, "overlay-input", AdjustCallback, p_sys );
    var_DelCallback( p_filter, "overlay-output", AdjustCallback, p_sys );
    vlc_mutex_destroy( &p_sys->lock );
    free( p_sys->psz_inputfile );
    free( p_sys->psz_outputfile );
    free( p_sys );
}

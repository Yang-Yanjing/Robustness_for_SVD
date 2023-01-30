/** Remove an extension from the activated list */
static int RemoveActivated( extensions_manager_t *p_mgr, extension_t *p_ext )
{
    if( p_mgr->p_sys->b_killed )
        return VLC_SUCCESS;
    vlc_mutex_lock( &p_mgr->p_sys->lock );
    int i_idx = -1;
    extension_t *p_iter;
    FOREACH_ARRAY( p_iter, p_mgr->p_sys->activated_extensions )
    {
        i_idx++;
        if( !p_iter )
        {
            i_idx = -1;
            break;
        }
        assert( p_iter->psz_name != NULL );
        if( !strcmp( p_iter->psz_name, p_ext->psz_name ) )
            break;
    }
    FOREACH_END()
    if( i_idx >= 0 )
    {
        ARRAY_REMOVE( p_mgr->p_sys->activated_extensions, i_idx );
    }
    else
    {
        msg_Dbg( p_mgr, "Can't find extension '%s' in the activated list",
                 p_ext->psz_title );
    }
    vlc_mutex_unlock( &p_mgr->p_sys->lock );
    return (i_idx >= 0) ? VLC_SUCCESS : VLC_EGENERIC;
}

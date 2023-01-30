 **/
bool IsActivated( extensions_manager_t *p_mgr, extension_t *p_ext )
{
    assert( p_ext != NULL );
    vlc_mutex_lock( &p_mgr->p_sys->lock );
    extension_t *p_iter;
    FOREACH_ARRAY( p_iter, p_mgr->p_sys->activated_extensions )
    {
        if( !p_iter )
            break;
        assert( p_iter->psz_name != NULL );
        if( !strcmp( p_iter->psz_name, p_ext->psz_name ) )
        {
            vlc_mutex_unlock( &p_mgr->p_sys->lock );
            return true;
        }
    }
    FOREACH_END()
    vlc_mutex_unlock( &p_mgr->p_sys->lock );
    return false;
}

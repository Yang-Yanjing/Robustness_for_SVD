}
static void DecSysRelease( decoder_sys_t *p_sys )
{
    vlc_mutex_lock( &p_sys->lock );
    p_sys->i_refcount--;
    if( p_sys->i_refcount > 0)
    {
        vlc_mutex_unlock( &p_sys->lock );
        return;
    }
    vlc_mutex_unlock( &p_sys->lock );
    vlc_mutex_destroy( &p_sys->lock );
#ifdef HAVE_TIGER
    if( p_sys->p_tr )
        tiger_renderer_destroy( p_sys->p_tr );
    free( p_sys->psz_tiger_default_font_desc );
#endif
    if (p_sys->b_ready)
        kate_clear( &p_sys->k );
    kate_info_clear( &p_sys->ki );
    kate_comment_clear( &p_sys->kc );
    free( p_sys );
}

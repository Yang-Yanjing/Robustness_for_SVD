 *****************************************************************************/
static void Destroy( es_out_t *p_out )
{
    es_out_sys_t *p_sys = p_out->p_sys;
    if( p_sys->b_delayed )
    {
        TsStop( p_sys->p_ts );
        p_sys->b_delayed = false;
    }
    while( p_sys->i_es > 0 )
        Del( p_out, p_sys->pp_es[0] );
    TAB_CLEAN( p_sys->i_es, p_sys->pp_es  );
    free( p_sys->psz_tmp_path );
    vlc_mutex_destroy( &p_sys->lock );
    free( p_sys );
    free( p_out );
}

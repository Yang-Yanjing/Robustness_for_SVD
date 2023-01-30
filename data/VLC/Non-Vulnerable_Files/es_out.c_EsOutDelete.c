 *****************************************************************************/
static void EsOutDelete( es_out_t *out )
{
    es_out_sys_t *p_sys = out->p_sys;
    assert( !p_sys->i_es && !p_sys->i_pgrm && !p_sys->p_pgrm );
    if( p_sys->ppsz_audio_language )
    {
        for( int i = 0; p_sys->ppsz_audio_language[i]; i++ )
            free( p_sys->ppsz_audio_language[i] );
        free( p_sys->ppsz_audio_language );
    }
    if( p_sys->ppsz_sub_language )
    {
        for( int i = 0; p_sys->ppsz_sub_language[i]; i++ )
            free( p_sys->ppsz_sub_language[i] );
        free( p_sys->ppsz_sub_language );
    }
    vlc_mutex_destroy( &p_sys->lock );
    free( p_sys );
    free( out );
}

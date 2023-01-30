 *****************************************************************************/
static void TsDestroy( ts_thread_t *p_ts )
{
    vlc_cond_destroy( &p_ts->wait );
    vlc_mutex_destroy( &p_ts->lock );
    free( p_ts );
}

 *****************************************************************************/
playlist_preparser_t *playlist_preparser_New( vlc_object_t *parent )
{
    playlist_preparser_t *p_preparser = malloc( sizeof(*p_preparser) );
    if( !p_preparser )
        return NULL;
    p_preparser->object = parent;
    p_preparser->p_fetcher = playlist_fetcher_New( parent );
    if( unlikely(p_preparser->p_fetcher == NULL) )
        msg_Err( parent, "cannot create fetcher" );
    vlc_mutex_init( &p_preparser->lock );
    vlc_cond_init( &p_preparser->wait );
    p_preparser->b_live = false;
    p_preparser->i_waiting = 0;
    p_preparser->pp_waiting = NULL;
    return p_preparser;
}

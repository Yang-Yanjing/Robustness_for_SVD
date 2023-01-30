 */
static void *Thread( void *data )
{
    playlist_preparser_t *p_preparser = data;
    vlc_object_t *obj = p_preparser->object;
    for( ;; )
    {
        input_item_t *p_current;
        /* */
        vlc_mutex_lock( &p_preparser->lock );
        if( p_preparser->i_waiting > 0 )
        {
            p_current = p_preparser->pp_waiting[0];
            REMOVE_ELEM( p_preparser->pp_waiting, p_preparser->i_waiting, 0 );
        }
        else
        {
            p_current = NULL;
            p_preparser->b_live = false;
            vlc_cond_signal( &p_preparser->wait );
        }
        vlc_mutex_unlock( &p_preparser->lock );
        if( !p_current )
            break;
        Preparse( obj, p_current );
        Art( p_preparser, p_current );
        vlc_gc_decref(p_current);
    }
    return NULL;
}

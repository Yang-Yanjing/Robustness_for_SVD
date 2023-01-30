 *****************************************************************************/
static void input_item_changed( const vlc_event_t * p_event,
                                void * user_data )
{
    playlist_item_t *p_item = user_data;
    VLC_UNUSED( p_event );
    var_SetAddress( p_item->p_playlist, "item-change", p_item->p_input );
}

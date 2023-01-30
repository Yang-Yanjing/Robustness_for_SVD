 */
int input_Preparse( vlc_object_t *p_parent, input_item_t *p_item )
{
    input_thread_t *p_input;
    /* Allocate descriptor */
    p_input = Create( p_parent, p_item, NULL, true, NULL );
    if( !p_input )
        return VLC_EGENERIC;
    if( !Init( p_input ) )
        End( p_input );
    vlc_object_release( p_input );
    return VLC_SUCCESS;
}

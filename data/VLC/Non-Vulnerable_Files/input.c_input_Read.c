 */
int input_Read( vlc_object_t *p_parent, input_item_t *p_item )
{
    input_thread_t *p_input = Create( p_parent, p_item, NULL, false, NULL );
    if( !p_input )
        return VLC_EGENERIC;
    if( !Init( p_input ) )
    {
        MainLoop( p_input, false );
        End( p_input );
    }
    vlc_object_release( p_input );
    return VLC_SUCCESS;
}

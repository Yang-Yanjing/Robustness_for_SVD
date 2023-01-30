 */
input_thread_t *input_CreateAndStart( vlc_object_t *p_parent,
                                      input_item_t *p_item, const char *psz_log )
{
    input_thread_t *p_input = input_Create( p_parent, p_item, psz_log, NULL );
    if( input_Start( p_input ) )
    {
        vlc_object_release( p_input );
        return NULL;
    }
    return p_input;
}

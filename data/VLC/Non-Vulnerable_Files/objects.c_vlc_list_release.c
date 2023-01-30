 *****************************************************************************/
void vlc_list_release( vlc_list_t *p_list )
{
    int i_index;
    for( i_index = 0; i_index < p_list->i_count; i_index++ )
    {
        vlc_object_release( p_list->p_values[i_index].p_object );
    }
    free( p_list->p_values );
    free( p_list );
}

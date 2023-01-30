 *****************************************************************************/
static void input_SubtitleAdd( input_thread_t *p_input,
                               const char *url, unsigned i_flags )
{
    input_source_t *sub = InputSourceNew( p_input );
    if( sub == NULL )
        return;
    vlc_value_t count;
    var_Change( p_input, "spu-es", VLC_VAR_CHOICESCOUNT, &count, NULL );
    if( InputSourceInit( p_input, sub, url, "subtitle",
                         (i_flags & SUB_CANFAIL) ) )
    {
        free( sub );
        return;
    }
    TAB_APPEND( p_input->p->i_slave, p_input->p->slave, sub );
    if( !(i_flags & SUB_FORCED) )
        return;
    /* Select the ES */
    vlc_value_t list;
    if( var_Change( p_input, "spu-es", VLC_VAR_GETLIST, &list, NULL ) )
        return;
    if( count.i_int == 0 )
        count.i_int++;
    /* if it was first one, there is disable too */
    if( count.i_int < list.p_list->i_count )
    {
        const int i_id = list.p_list->p_values[count.i_int].i_int;
        es_out_Control( p_input->p->p_es_out_display, ES_OUT_SET_ES_DEFAULT_BY_ID, i_id );
        es_out_Control( p_input->p->p_es_out_display, ES_OUT_SET_ES_BY_ID, i_id );
    }
    var_FreeList( &list, NULL );
}

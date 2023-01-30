 *****************************************************************************/
static int Send( sout_stream_t *p_stream, sout_stream_id_sys_t *id,
                 block_t *p_buffer )
{
    sout_stream_sys_t *p_sys = p_stream->p_sys;
    sout_stream_t     *p_dup_stream;
    int               i_stream;
    /* Loop through the linked list of buffers */
    while( p_buffer )
    {
        block_t *p_next = p_buffer->p_next;
        p_buffer->p_next = NULL;
        for( i_stream = 0; i_stream < p_sys->i_nb_streams - 1; i_stream++ )
        {
            p_dup_stream = p_sys->pp_streams[i_stream];
            if( id->pp_ids[i_stream] )
            {
                block_t *p_dup = block_Duplicate( p_buffer );
                if( p_dup )
                    sout_StreamIdSend( p_dup_stream, id->pp_ids[i_stream], p_dup );
            }
        }
        if( i_stream < p_sys->i_nb_streams && id->pp_ids[i_stream] )
        {
            p_dup_stream = p_sys->pp_streams[i_stream];
            sout_StreamIdSend( p_dup_stream, id->pp_ids[i_stream], p_buffer );
        }
        else
        {
            block_Release( p_buffer );
        }
        p_buffer = p_next;
    }
    return VLC_SUCCESS;
}

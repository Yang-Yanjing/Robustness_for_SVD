 *****************************************************************************/
static sout_stream_id_sys_t * Add( sout_stream_t *p_stream, es_format_t *p_fmt )
{
    sout_stream_sys_t *p_sys = p_stream->p_sys;
    sout_stream_id_sys_t  *id;
    int i_stream, i_valid_streams = 0;
    id = malloc( sizeof( sout_stream_id_sys_t ) );
    if( !id )
        return NULL;
    TAB_INIT( id->i_nb_ids, id->pp_ids );
    msg_Dbg( p_stream, "duplicated a new stream codec=%4.4s (es=%d group=%d)",
             (char*)&p_fmt->i_codec, p_fmt->i_id, p_fmt->i_group );
    for( i_stream = 0; i_stream < p_sys->i_nb_streams; i_stream++ )
    {
        void *id_new = NULL;
        if( ESSelected( p_fmt, p_sys->ppsz_select[i_stream] ) )
        {
            sout_stream_t *out = p_sys->pp_streams[i_stream];
            id_new = (void*)sout_StreamIdAdd( out, p_fmt );
            if( id_new )
            {
                msg_Dbg( p_stream, "    - added for output %d", i_stream );
                i_valid_streams++;
            }
            else
            {
                msg_Dbg( p_stream, "    - failed for output %d", i_stream );
            }
        }
        else
        {
            msg_Dbg( p_stream, "    - ignored for output %d", i_stream );
        }
        /* Append failed attempts as well to keep track of which pp_id
         * belongs to which duplicated stream */
        TAB_APPEND( id->i_nb_ids, id->pp_ids, id_new );
    }
    if( i_valid_streams <= 0 )
    {
        Del( p_stream, id );
        return NULL;
    }
    return id;
}

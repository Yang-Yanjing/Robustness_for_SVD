 *****************************************************************************/
static int Del( sout_stream_t *p_stream, sout_stream_id_sys_t *id )
{
    sout_stream_sys_t *p_sys = p_stream->p_sys;
    int               i_stream;
    for( i_stream = 0; i_stream < p_sys->i_nb_streams; i_stream++ )
    {
        if( id->pp_ids[i_stream] )
        {
            sout_stream_t *out = p_sys->pp_streams[i_stream];
            sout_StreamIdDel( out, id->pp_ids[i_stream] );
        }
    }
    free( id->pp_ids );
    free( id );
    return VLC_SUCCESS;
}

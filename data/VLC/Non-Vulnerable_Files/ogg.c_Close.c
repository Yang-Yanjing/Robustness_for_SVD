 *****************************************************************************/
static void Close( vlc_object_t * p_this )
{
    sout_mux_t     *p_mux = (sout_mux_t*)p_this;
    sout_mux_sys_t *p_sys = p_mux->p_sys;
    ogg_stream_t *p_stream;
    msg_Info( p_mux, "Close" );
    if( p_sys->i_del_streams )
    {
        /* Close the current ogg stream */
        msg_Dbg( p_mux, "writing footers" );
        for(int i = 0; i < p_mux->i_nb_inputs; i++ )
        {
            p_stream = (ogg_stream_t *) p_mux->pp_inputs[i]->p_sys;
            OggCreateStreamFooter( p_mux, p_stream );
            free( p_stream->skeleton.p_index );
        }
        /* Remove deleted logical streams */
        for(int i = 0; i < p_sys->i_del_streams; i++ )
        {
            OggCreateStreamFooter( p_mux, p_sys->pp_del_streams[i] );
            free( p_sys->pp_del_streams[i]->p_oggds_header );
            free( p_sys->pp_del_streams[i]->skeleton.p_index );
            free( p_sys->pp_del_streams[i] );
        }
        free( p_sys->pp_del_streams );
        p_sys->i_streams -= p_sys->i_del_streams;
    }
    /* rewrite fishead with final values */
    if ( p_sys->skeleton.b_create && p_sys->skeleton.b_head_done )
    {
        OggRewriteFisheadPage( p_mux );
    }
    free( p_sys );
}

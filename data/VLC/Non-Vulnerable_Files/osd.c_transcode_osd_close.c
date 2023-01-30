}
void transcode_osd_close( sout_stream_t *p_stream, sout_stream_id_sys_t *id)
{
    sout_stream_sys_t *p_sys = p_stream->p_sys;
    /* Close encoder */
    if( id )
    {
        if( id->p_encoder->p_module )
            module_unneed( id->p_encoder, id->p_encoder->p_module );
    }
    p_sys->b_osd = false;
}

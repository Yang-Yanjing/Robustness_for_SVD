 */
static void DeleteDecoder( decoder_t * p_dec )
{
    decoder_owner_sys_t *p_owner = p_dec->p_owner;
    msg_Dbg( p_dec, "killing decoder fourcc `%4.4s', %u PES in FIFO",
             (char*)&p_dec->fmt_in.i_codec,
             (unsigned)block_FifoCount( p_owner->p_fifo ) );
    /* Free all packets still in the decoder fifo. */
    block_FifoEmpty( p_owner->p_fifo );
    block_FifoRelease( p_owner->p_fifo );
    /* Cleanup */
    if( p_owner->p_aout )
    {
        /* TODO: REVISIT gap-less audio */
        aout_DecFlush( p_owner->p_aout );
        aout_DecDelete( p_owner->p_aout );
        input_resource_PutAout( p_owner->p_resource, p_owner->p_aout );
        if( p_owner->p_input != NULL )
            input_SendEventAout( p_owner->p_input );
    }
    if( p_owner->p_vout )
    {
        /* Hack to make sure all the the pictures are freed by the decoder
         * and that the vout is not paused anymore */
        vout_Reset( p_owner->p_vout );
        /* */
        input_resource_RequestVout( p_owner->p_resource, p_owner->p_vout, NULL,
                                    0, true );
        if( p_owner->p_input != NULL )
            input_SendEventVout( p_owner->p_input );
    }
#ifdef ENABLE_SOUT
    if( p_owner->p_sout_input )
    {
        sout_InputDelete( p_owner->p_sout_input );
        es_format_Clean( &p_owner->sout );
    }
#endif
    if( p_dec->fmt_out.i_cat == SPU_ES )
    {
        vout_thread_t *p_vout = input_resource_HoldVout( p_owner->p_resource );
        if( p_vout )
        {
            if( p_owner->p_spu_vout == p_vout )
                vout_FlushSubpictureChannel( p_vout, p_owner->i_spu_channel );
            vlc_object_release( p_vout );
        }
    }
    es_format_Clean( &p_dec->fmt_in );
    es_format_Clean( &p_dec->fmt_out );
    if( p_dec->p_description )
        vlc_meta_Delete( p_dec->p_description );
    es_format_Clean( &p_owner->fmt_description );
    if( p_owner->p_description )
        vlc_meta_Delete( p_owner->p_description );
    if( p_owner->p_packetizer )
    {
        module_unneed( p_owner->p_packetizer,
                       p_owner->p_packetizer->p_module );
        es_format_Clean( &p_owner->p_packetizer->fmt_in );
        es_format_Clean( &p_owner->p_packetizer->fmt_out );
        if( p_owner->p_packetizer->p_description )
            vlc_meta_Delete( p_owner->p_packetizer->p_description );
        vlc_object_release( p_owner->p_packetizer );
    }
    vlc_cond_destroy( &p_owner->wait_acknowledge );
    vlc_cond_destroy( &p_owner->wait_request );
    vlc_mutex_destroy( &p_owner->lock );
    vlc_object_release( p_dec );
    free( p_owner );
}

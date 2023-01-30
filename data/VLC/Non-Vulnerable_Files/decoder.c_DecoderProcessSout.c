 */
static void DecoderProcessSout( decoder_t *p_dec, block_t *p_block )
{
    decoder_owner_sys_t *p_owner = (decoder_owner_sys_t *)p_dec->p_owner;
    block_t *p_sout_block;
    while( ( p_sout_block =
                 p_dec->pf_packetize( p_dec, p_block ? &p_block : NULL ) ) )
    {
        if( !p_owner->p_sout_input )
        {
            es_format_Copy( &p_owner->sout, &p_dec->fmt_out );
            p_owner->sout.i_group = p_dec->fmt_in.i_group;
            p_owner->sout.i_id = p_dec->fmt_in.i_id;
            if( p_dec->fmt_in.psz_language )
            {
                free( p_owner->sout.psz_language );
                p_owner->sout.psz_language =
                    strdup( p_dec->fmt_in.psz_language );
            }
            p_owner->p_sout_input =
                sout_InputNew( p_owner->p_sout,
                               &p_owner->sout );
            if( p_owner->p_sout_input == NULL )
            {
                msg_Err( p_dec, "cannot create packetizer output (%4.4s)",
                         (char *)&p_owner->sout.i_codec );
                p_dec->b_error = true;
                block_ChainRelease(p_sout_block);
                break;
            }
        }
        while( p_sout_block )
        {
            block_t *p_next = p_sout_block->p_next;
            p_sout_block->p_next = NULL;
            if( DecoderPlaySout( p_dec, p_sout_block ) == VLC_EGENERIC )
            {
                msg_Err( p_dec, "cannot continue streaming due to errors" );
                p_dec->b_error = true;
                /* Cleanup */
                block_ChainRelease( p_next );
                return;
            }
            p_sout_block = p_next;
        }
    }
}

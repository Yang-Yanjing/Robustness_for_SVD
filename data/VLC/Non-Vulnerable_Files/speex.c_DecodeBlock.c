 ****************************************************************************/
static block_t *DecodeBlock( decoder_t *p_dec, block_t **pp_block )
{
    decoder_sys_t *p_sys = p_dec->p_sys;
    ogg_packet oggpacket;
    if( !pp_block ) return NULL;
    block_t *block = *pp_block;
    if( block != NULL )
    {
        /* Block to Ogg packet */
        oggpacket.packet = block->p_buffer;
        oggpacket.bytes = block->i_buffer;
    }
    else
    {
        if( p_sys->b_packetizer ) return NULL;
        /* Block to Ogg packet */
        oggpacket.packet = NULL;
        oggpacket.bytes = 0;
    }
    oggpacket.granulepos = -1;
    oggpacket.b_o_s = 0;
    oggpacket.e_o_s = 0;
    oggpacket.packetno = 0;
    /* Check for headers */
    if( !p_sys->b_has_headers )
    {
        if( !p_dec->fmt_in.p_extra )
        {
            msg_Warn( p_dec, "Header missing, using default settings" );
            if( CreateDefaultHeader( p_dec ) )
            {
                if( block != NULL )
                    block_Release( block );
                return NULL;
            }
        }
        else if( ProcessHeaders( p_dec ) )
        {
            if( block != NULL )
                block_Release( block );
            return NULL;
        }
        p_sys->b_has_headers = true;
    }
    return ProcessPacket( p_dec, &oggpacket, pp_block );
}

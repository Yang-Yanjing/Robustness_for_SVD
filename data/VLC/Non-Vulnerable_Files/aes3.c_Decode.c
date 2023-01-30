 *****************************************************************************/
static block_t *Decode( decoder_t *p_dec, block_t **pp_block )
{
    decoder_sys_t *p_sys = p_dec->p_sys;
    block_t       *p_block;
    block_t       *p_aout_buffer;
    int            i_frame_length, i_bits;
    p_block = Parse( p_dec, &i_frame_length, &i_bits, pp_block, false );
    if( !p_block )
        return NULL;
    p_aout_buffer = decoder_NewAudioBuffer( p_dec, i_frame_length );
    if( p_aout_buffer == NULL )
        goto exit;
    p_aout_buffer->i_pts = date_Get( &p_sys->end_date );
    p_aout_buffer->i_length = date_Increment( &p_sys->end_date,
                                      i_frame_length ) - p_aout_buffer->i_pts;
    p_block->i_buffer -= AES3_HEADER_LEN;
    p_block->p_buffer += AES3_HEADER_LEN;
    if( i_bits == 24 )
    {
        uint32_t *p_out = (uint32_t *)p_aout_buffer->p_buffer;
        while( p_block->i_buffer / 7 )
        {
            *(p_out++) =  (reverse[p_block->p_buffer[0]] <<  8)
                        | (reverse[p_block->p_buffer[1]] << 16)
                        | (reverse[p_block->p_buffer[2]] << 24);
            *(p_out++) = ((reverse[p_block->p_buffer[3]] <<  4)
                        | (reverse[p_block->p_buffer[4]] << 12)
                        | (reverse[p_block->p_buffer[5]] << 20)
                        | (reverse[p_block->p_buffer[6]] << 28)) & 0xFFFFFF00;
            p_block->i_buffer -= 7;
            p_block->p_buffer += 7;
        }
    }
    else if( i_bits == 20 )
    {
        uint32_t *p_out = (uint32_t *)p_aout_buffer->p_buffer;
        while( p_block->i_buffer / 6 )
        {
            *(p_out++) = (reverse[p_block->p_buffer[0]] << 12)
                       | (reverse[p_block->p_buffer[1]] << 20)
                       | (reverse[p_block->p_buffer[2]] << 28);
            *(p_out++) = (reverse[p_block->p_buffer[3]] << 12)
                       | (reverse[p_block->p_buffer[4]] << 20)
                       | (reverse[p_block->p_buffer[5]] << 28);
            p_block->i_buffer -= 6;
            p_block->p_buffer += 6;
        }
    }
    else
    {
        uint16_t *p_out = (uint16_t *)p_aout_buffer->p_buffer;
        assert( i_bits == 16 );
        while( p_block->i_buffer / 5 )
        {
            *(p_out++) =  reverse[p_block->p_buffer[0]]
                        |(reverse[p_block->p_buffer[1]] <<  8);
            *(p_out++) = (reverse[p_block->p_buffer[2]] >>  4)
                       | (reverse[p_block->p_buffer[3]] <<  4)
                       | (reverse[p_block->p_buffer[4]] << 12);
            p_block->i_buffer -= 5;
            p_block->p_buffer += 5;
        }
    }
exit:
    block_Release( p_block );
    return p_aout_buffer;
}

}
static void AobExtract( block_t *p_aout_buffer,
                        block_t *p_block, unsigned i_bits, aob_group_t p_group[2] )
{
    const unsigned i_channels = p_group[0].i_channels +
                                ( p_group[1].b_used ? p_group[1].i_channels : 0 );
    uint8_t *p_out = p_aout_buffer->p_buffer;
    while( p_block->i_buffer > 0 )
    {
        for( int i = 0; i < 2; i++ )
        {
            const aob_group_t *g = &p_group[1-i];
            const unsigned int i_group_size = 2 * g->i_channels * i_bits / 8;
            if( p_block->i_buffer < i_group_size )
            {
                p_block->i_buffer = 0;
                break;
            }
            for( unsigned n = 0; n < 2; n++ )
            {
                for( unsigned j = 0; j < g->i_channels && g->b_used; j++ )
                {
                    const int i_src = n * g->i_channels + j;
                    const int i_dst = n * i_channels + g->pi_position[j];
                    if( i_bits == 24 )
                    {
                        p_out[3*i_dst+0] = p_block->p_buffer[2*i_src+0];
                        p_out[3*i_dst+1] = p_block->p_buffer[2*i_src+1];
                        p_out[3*i_dst+2] = p_block->p_buffer[4*g->i_channels+i_src];
                    }
                    else if( i_bits == 20 )
                    {
                        p_out[3*i_dst+0] = p_block->p_buffer[2*i_src+0];
                        p_out[3*i_dst+1] = p_block->p_buffer[2*i_src+1];
                        if( n == 0 )
                            p_out[3*i_dst+2] = (p_block->p_buffer[4*g->i_channels+i_src]     ) & 0xf0;
                        else
                            p_out[3*i_dst+2] = (p_block->p_buffer[4*g->i_channels+i_src] << 4) & 0xf0;
                    }
                    else
                    {
                        assert( i_bits == 16 );
                        p_out[2*i_dst+0] = p_block->p_buffer[2*i_src+0];
                        p_out[2*i_dst+1] = p_block->p_buffer[2*i_src+1];
                    }
                }
            }
            p_block->i_buffer -= i_group_size;
            p_block->p_buffer += i_group_size;
        }
        /* */
        p_out += (i_bits == 16 ? 2 : 3) * i_channels * 2;
    }
}

 *****************************************************************************/
static block_t *SendFrame( decoder_t *p_dec, block_t **pp_block )
{
    block_t *p_block = DecodeBlock( p_dec, pp_block );
    if( p_block == NULL )
        return NULL;
    decoder_sys_t *p_sys = p_dec->p_sys;
    /* Date management: 1 frame per packet */
    p_block->i_dts = p_block->i_pts = date_Get( &p_sys->pts );
    date_Increment( &p_sys->pts, 1 );
    if( p_sys->b_invert )
    {
        block_t *out = block_Alloc( p_block->i_buffer );
        if( likely(out != NULL) )
        {
            block_CopyProperties( out, p_block );
            const uint8_t *p_src = p_block->p_buffer;
            uint8_t *p_pixels = out->p_buffer;
            for( unsigned i = 0; i < PICTURE_PLANE_MAX; i++ )
            {
                unsigned pitch = p_sys->pitches[i];
                unsigned lines = p_sys->lines[i];
                uint8_t *p_dst = p_pixels + (pitch * lines);
                for( unsigned x = 0; x < lines; x++ )
                {
                    p_dst -= p_sys->pitches[i];
                    memcpy( p_dst, p_src, p_sys->pitches[i] );
                    p_src += p_sys->pitches[i];
                }
            }
        }
        block_Release( p_block );
        p_block = out;
    }
    return p_block;
}

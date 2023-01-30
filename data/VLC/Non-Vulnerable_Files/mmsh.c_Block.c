 *****************************************************************************/
static block_t *Block( access_t *p_access )
{
    access_sys_t *p_sys = p_access->p_sys;
    const unsigned i_packet_min = p_sys->asfh.i_min_data_packet_size;
    if( p_access->info.i_pos < p_sys->i_start + p_sys->i_header )
    {
        const size_t i_offset = p_access->info.i_pos - p_sys->i_start;
        const size_t i_copy = p_sys->i_header - i_offset;
        block_t *p_block = block_Alloc( i_copy );
        if( !p_block )
            return NULL;
        memcpy( p_block->p_buffer, &p_sys->p_header[i_offset], i_copy );
        p_access->info.i_pos += i_copy;
        return p_block;
    }
    else if( p_sys->i_packet_length > 0 &&
             p_sys->i_packet_used < __MAX( p_sys->i_packet_length, i_packet_min ) )
    {
        size_t i_copy = 0;
        size_t i_padding = 0;
        if( p_sys->i_packet_used < p_sys->i_packet_length )
            i_copy = p_sys->i_packet_length - p_sys->i_packet_used;
        if( __MAX( p_sys->i_packet_used, p_sys->i_packet_length ) < i_packet_min )
            i_padding = i_packet_min - __MAX( p_sys->i_packet_used, p_sys->i_packet_length );
        block_t *p_block = block_Alloc( i_copy + i_padding );
        if( !p_block )
            return NULL;
        if( i_copy > 0 )
            memcpy( &p_block->p_buffer[0], &p_sys->p_packet[p_sys->i_packet_used], i_copy );
        if( i_padding > 0 )
            memset( &p_block->p_buffer[i_copy], 0, i_padding );
        p_sys->i_packet_used += i_copy + i_padding;
        p_access->info.i_pos += i_copy + i_padding;
        return p_block;
    }
    chunk_t ck;
    if( GetPacket( p_access, &ck ) )
    {
        int i_ret = -1;
        if( p_sys->b_broadcast )
        {
            if( (ck.i_type == 0x4524) && (ck.i_sequence != 0) )
                i_ret = Restart( p_access );
            else if( ck.i_type == 0x4324 )
                i_ret = Reset( p_access );
        }
        if( i_ret )
        {
            p_access->info.b_eof = true;
            return 0;
        }
    }
    if( ck.i_type != 0x4424 )
    {
        p_sys->i_packet_used = 0;
        p_sys->i_packet_length = 0;
    }
    return NULL;
}

 *****************************************************************************/
static block_t *DoWork( filter_t * p_filter, block_t * p_in_buf )
{
    uint32_t i_ac5_spdif_type = 0;
    uint16_t i_fz = p_in_buf->i_nb_samples * 4;
    uint16_t i_frame, i_length = p_in_buf->i_buffer;
    static const uint8_t p_sync_le[6] = { 0x72, 0xF8, 0x1F, 0x4E, 0x00, 0x00 };
    static const uint8_t p_sync_be[6] = { 0xF8, 0x72, 0x4E, 0x1F, 0x00, 0x00 };
    if( p_in_buf->i_buffer != p_filter->p_sys->i_frame_size )
    {
        /* Frame size changed, reset everything */
        msg_Warn( p_filter, "Frame size changed from %zu to %zu, "
                          "resetting everything.",
                  p_filter->p_sys->i_frame_size, p_in_buf->i_buffer );
        p_filter->p_sys->i_frame_size = p_in_buf->i_buffer;
        p_filter->p_sys->p_buf = xrealloc( p_filter->p_sys->p_buf,
                                                  p_in_buf->i_buffer * 3 );
        p_filter->p_sys->i_frames = 0;
    }
    /* Backup frame */
    /* TODO: keeping the blocks in a list would save one memcpy */
    memcpy( p_filter->p_sys->p_buf + p_in_buf->i_buffer *
                  p_filter->p_sys->i_frames,
                p_in_buf->p_buffer, p_in_buf->i_buffer );
    p_filter->p_sys->i_frames++;
    if( p_filter->p_sys->i_frames < 3 )
    {
        if( p_filter->p_sys->i_frames == 1 )
            /* We'll need the starting date */
            p_filter->p_sys->start_date = p_in_buf->i_pts;
        /* Not enough data */
        block_Release( p_in_buf );
        return NULL;
    }
    p_filter->p_sys->i_frames = 0;
    block_t *p_out_buf = block_Alloc( 12 * p_in_buf->i_nb_samples );
    if( !p_out_buf )
        goto out;
    for( i_frame = 0; i_frame < 3; i_frame++ )
    {
        uint16_t i_length_padded = i_length;
        uint8_t * p_out = p_out_buf->p_buffer + (i_frame * i_fz);
        uint8_t * p_in = p_filter->p_sys->p_buf + (i_frame * i_length);
        switch( p_in_buf->i_nb_samples )
        {
            case  512: i_ac5_spdif_type = 0x0B; break;
            case 1024: i_ac5_spdif_type = 0x0C; break;
            case 2048: i_ac5_spdif_type = 0x0D; break;
        }
        /* Copy the S/PDIF headers. */
        if( p_filter->fmt_out.audio.i_format == VLC_CODEC_SPDIFB )
        {
            memcpy( p_out, p_sync_be, 6 );
            p_out[5] = i_ac5_spdif_type;
            SetWBE( p_out + 6, i_length << 3 );
        }
        else
        {
            memcpy( p_out, p_sync_le, 6 );
            p_out[4] = i_ac5_spdif_type;
            SetWLE( p_out + 6, i_length << 3 );
        }
        if( ( (p_in[0] == 0x1F || p_in[0] == 0x7F) && p_filter->fmt_out.audio.i_format == VLC_CODEC_SPDIFL ) ||
            ( (p_in[0] == 0xFF || p_in[0] == 0xFE) && p_filter->fmt_out.audio.i_format == VLC_CODEC_SPDIFB ) )
        {
            /* We are dealing with a big endian bitstream and a little endian output
             * or a little endian bitstream and a big endian output.
             * Byteswap the stream */
            swab( p_in, p_out + 8, i_length );
            /* If i_length is odd, we have to adjust swapping a bit.. */
            if( i_length & 1 )
            {
                p_out[8+i_length-1] = 0;
                p_out[8+i_length] = p_in[i_length-1];
                i_length_padded++;
            }
        }
        else
        {
            memcpy( p_out + 8, p_in, i_length );
        }
        if( i_fz > i_length + 8 )
        {
            memset( p_out + 8 + i_length_padded, 0,
                        i_fz - i_length_padded - 8 );
        }
    }
    p_out_buf->i_pts = p_filter->p_sys->start_date;
    p_out_buf->i_nb_samples = p_in_buf->i_nb_samples * 3;
    p_out_buf->i_buffer = p_out_buf->i_nb_samples * 4;
out:
    block_Release( p_in_buf );
    return p_out_buf;
}

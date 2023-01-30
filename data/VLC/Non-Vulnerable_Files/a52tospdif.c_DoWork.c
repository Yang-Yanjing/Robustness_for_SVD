 *****************************************************************************/
static block_t *DoWork( filter_t * p_filter, block_t *p_in_buf )
{
    /* AC3 is natively big endian. Most SPDIF devices have the native
     * endianness of the computer system.
     * On Mac OS X however, little endian devices are also common.
     */
    static const uint8_t p_sync_le[6] = { 0x72, 0xF8, 0x1F, 0x4E, 0x01, 0x00 };
    static const uint8_t p_sync_be[6] = { 0xF8, 0x72, 0x4E, 0x1F, 0x00, 0x01 };
    uint16_t i_frame_size = p_in_buf->i_buffer / 2;
    uint8_t * p_in = p_in_buf->p_buffer;
    block_t *p_out_buf = block_Alloc( AOUT_SPDIF_SIZE );
    if( !p_out_buf )
        goto out;
    uint8_t * p_out = p_out_buf->p_buffer;
    /* Copy the S/PDIF headers. */
    if( p_filter->fmt_out.audio.i_format == VLC_CODEC_SPDIFB )
    {
        memcpy( p_out, p_sync_be, 6 );
        p_out[4] = p_in[5] & 0x7; /* bsmod */
        SetWBE( p_out + 6, i_frame_size << 4 );
        memcpy( &p_out[8], p_in, i_frame_size * 2 );
    }
    else
    {
        memcpy( p_out, p_sync_le, 6 );
        p_out[5] = p_in[5] & 0x7; /* bsmod */
        SetWLE( p_out + 6, i_frame_size << 4 );
        swab( p_in, &p_out[8], i_frame_size * 2 );
    }
    memset( p_out + 8 + i_frame_size * 2, 0,
                AOUT_SPDIF_SIZE - i_frame_size * 2 - 8 );
    p_out_buf->i_dts = p_in_buf->i_dts;
    p_out_buf->i_pts = p_in_buf->i_pts;
    p_out_buf->i_nb_samples = p_in_buf->i_nb_samples;
    p_out_buf->i_buffer = AOUT_SPDIF_SIZE;
out:
    block_Release( p_in_buf );
    return p_out_buf;
}

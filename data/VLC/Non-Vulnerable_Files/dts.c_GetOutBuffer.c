 *****************************************************************************/
static uint8_t *GetOutBuffer( decoder_t *p_dec, block_t **pp_out_buffer )
{
    decoder_sys_t *p_sys = p_dec->p_sys;
    uint8_t *p_buf;
    if( p_dec->fmt_out.audio.i_rate != p_sys->i_rate )
    {
        msg_Info( p_dec, "DTS channels:%d samplerate:%d bitrate:%d",
                  p_sys->i_channels, p_sys->i_rate, p_sys->i_bit_rate );
        date_Init( &p_sys->end_date, p_sys->i_rate, 1 );
        date_Set( &p_sys->end_date, p_sys->i_pts );
    }
    p_dec->fmt_out.audio.i_rate     = p_sys->i_rate;
    p_dec->fmt_out.audio.i_channels = p_sys->i_channels;
    /* Hack for DTS S/PDIF filter which needs to pad the DTS frames */
    p_dec->fmt_out.audio.i_bytes_per_frame =
        __MAX( p_sys->i_frame_size, p_sys->i_frame_length * 4 );
    p_dec->fmt_out.audio.i_frame_length = p_sys->i_frame_length;
    p_dec->fmt_out.audio.i_original_channels = p_sys->i_channels_conf;
    p_dec->fmt_out.audio.i_physical_channels =
        p_sys->i_channels_conf & AOUT_CHAN_PHYSMASK;
    p_dec->fmt_out.i_bitrate = p_sys->i_bit_rate;
    if( p_sys->b_packetizer )
    {
        block_t *p_sout_buffer = GetSoutBuffer( p_dec );
        p_buf = p_sout_buffer ? p_sout_buffer->p_buffer : NULL;
        *pp_out_buffer = p_sout_buffer;
    }
    else
    {
        block_t *p_aout_buffer = GetAoutBuffer( p_dec );
        p_buf = p_aout_buffer ? p_aout_buffer->p_buffer : NULL;
        *pp_out_buffer = p_aout_buffer;
    }
    return p_buf;
}

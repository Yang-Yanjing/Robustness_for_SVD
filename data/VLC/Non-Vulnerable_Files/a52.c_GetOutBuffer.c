 *****************************************************************************/
static uint8_t *GetOutBuffer( decoder_t *p_dec, block_t **pp_out_buffer )
{
    decoder_sys_t *p_sys = p_dec->p_sys;
    uint8_t *p_buf;
    if( p_dec->fmt_out.audio.i_rate != p_sys->frame.i_rate )
    {
        msg_Dbg( p_dec, "A/52 channels:%d samplerate:%d bitrate:%d",
                 p_sys->frame.i_channels, p_sys->frame.i_rate, p_sys->frame.i_bitrate );
        date_Init( &p_sys->end_date, p_sys->frame.i_rate, 1 );
        date_Set( &p_sys->end_date, p_sys->i_pts );
    }
    p_dec->fmt_out.audio.i_rate     = p_sys->frame.i_rate;
    p_dec->fmt_out.audio.i_channels = p_sys->frame.i_channels;
    if( p_dec->fmt_out.audio.i_bytes_per_frame < p_sys->frame.i_size )
        p_dec->fmt_out.audio.i_bytes_per_frame = p_sys->frame.i_size;
    p_dec->fmt_out.audio.i_frame_length = p_sys->frame.i_samples;
    p_dec->fmt_out.audio.i_original_channels = p_sys->frame.i_channels_conf;
    p_dec->fmt_out.audio.i_physical_channels =
        p_sys->frame.i_channels_conf & AOUT_CHAN_PHYSMASK;
    p_dec->fmt_out.i_bitrate = p_sys->frame.i_bitrate;
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

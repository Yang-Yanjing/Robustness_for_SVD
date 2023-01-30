}
static void OggFillVP8Header( uint8_t *p_buffer, sout_input_t *p_input )
{
    memcpy( p_buffer, "OVP80\x01\x01\x00", 8 );
    SetWBE( &p_buffer[8], p_input->p_fmt->video.i_width );
    SetDWBE( &p_buffer[14], p_input->p_fmt->video.i_sar_den );/* 24 bits, 15~ */
    SetDWBE( &p_buffer[11], p_input->p_fmt->video.i_sar_num );/* 24 bits, 12~ */
    SetWBE( &p_buffer[10], p_input->p_fmt->video.i_height );
    SetDWBE( &p_buffer[18], p_input->p_fmt->video.i_frame_rate );
    SetDWBE( &p_buffer[22], p_input->p_fmt->video.i_frame_rate_base );
}

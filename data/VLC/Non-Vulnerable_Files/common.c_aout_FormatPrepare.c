 *****************************************************************************/
void aout_FormatPrepare( audio_sample_format_t * p_format )
{
    p_format->i_channels = aout_FormatNbChannels( p_format );
    p_format->i_bitspersample = aout_BitsPerSample( p_format->i_format );
    if( p_format->i_bitspersample > 0 )
    {
        p_format->i_bytes_per_frame = ( p_format->i_bitspersample / 8 )
                                    * aout_FormatNbChannels( p_format );
        p_format->i_frame_length = 1;
    }
}

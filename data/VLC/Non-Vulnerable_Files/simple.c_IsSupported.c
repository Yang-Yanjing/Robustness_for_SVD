 *****************************************************************************/
static bool IsSupported( const audio_format_t *p_input, const audio_format_t *p_output )
{
    if( p_input->i_format != VLC_CODEC_FL32 ||
        p_input->i_format != p_output->i_format ||
        p_input->i_rate != p_output->i_rate )
    {
        return false;
    }
    if( p_input->i_physical_channels == p_output->i_physical_channels &&
        p_input->i_original_channels == p_output->i_original_channels )
    {
        return false;
    }
    /* Only conversion to Mono, Stereo, 4.0 and 5.1 */
    if( p_output->i_physical_channels != AOUT_CHAN_CENTER &&
        p_output->i_physical_channels != AOUT_CHANS_2_0 &&
        p_output->i_physical_channels != AOUT_CHANS_4_0 &&
        p_output->i_physical_channels != AOUT_CHANS_5_1 )
    {
        return false;
    }
    /* Only from 7.x/5.x/4.0/3.x/2.0
     * NB 5.X rear and middle are handled the same way
     * We don't support 2.1 -> 2.0 (trivial can do it)
     * TODO: We don't support any 8.1 input
     * TODO: We don't support any 6.x input
     * TODO: We don't support 4.0 rear and 4.0 middle
     * */
    if( (p_input->i_physical_channels & ~AOUT_CHAN_LFE) != AOUT_CHANS_7_0 &&
        (p_input->i_physical_channels)                  != AOUT_CHANS_6_1_MIDDLE &&
        (p_input->i_physical_channels & ~AOUT_CHAN_LFE) != AOUT_CHANS_5_0 &&
        (p_input->i_physical_channels & ~AOUT_CHAN_LFE) != AOUT_CHANS_5_0_MIDDLE &&
        (p_input->i_physical_channels & ~AOUT_CHAN_LFE) != AOUT_CHANS_4_CENTER_REAR &&
        (p_input->i_physical_channels & ~AOUT_CHAN_LFE) != AOUT_CHANS_3_0 &&
         p_input->i_physical_channels != AOUT_CHANS_2_0 )
    {
        return false;
    }
    /* Only downmixing */
    if( aout_FormatNbChannels( p_input ) <= aout_FormatNbChannels( p_output ) )
        return false;
    return true;
}

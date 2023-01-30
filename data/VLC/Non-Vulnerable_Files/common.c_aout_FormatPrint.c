 */
void aout_FormatPrint( vlc_object_t *obj, const char *psz_text,
                       const audio_sample_format_t *p_format )
{
    msg_Dbg( obj, "%s '%4.4s' %d Hz %s frame=%d samples/%d bytes", psz_text,
             (char *)&p_format->i_format, p_format->i_rate,
             aout_FormatPrintChannels( p_format ),
             p_format->i_frame_length, p_format->i_bytes_per_frame );
}

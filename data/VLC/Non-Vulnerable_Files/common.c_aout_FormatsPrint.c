 */
void aout_FormatsPrint( vlc_object_t *obj, const char * psz_text,
                        const audio_sample_format_t * p_format1,
                        const audio_sample_format_t * p_format2 )
{
    msg_Dbg( obj, "%s '%4.4s'->'%4.4s' %d Hz->%d Hz %s->%s",
             psz_text,
             (char *)&p_format1->i_format, (char *)&p_format2->i_format,
             p_format1->i_rate, p_format2->i_rate,
             aout_FormatPrintChannels( p_format1 ),
             aout_FormatPrintChannels( p_format2 ) );
}

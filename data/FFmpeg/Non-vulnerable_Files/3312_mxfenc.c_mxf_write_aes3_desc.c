static void mxf_write_aes3_desc(AVFormatContext *s, AVStream *st)
{
    mxf_write_wav_common(s, st, mxf_aes3_descriptor_key, 0);
}

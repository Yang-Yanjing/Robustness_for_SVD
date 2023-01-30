static void mxf_write_wav_desc(AVFormatContext *s, AVStream *st)
{
    mxf_write_wav_common(s, st, mxf_wav_descriptor_key, 0);
}

static void mxf_write_generic_sound_desc(AVFormatContext *s, AVStream *st)
{
    mxf_write_generic_sound_common(s, st, mxf_generic_sound_descriptor_key, 0);
}

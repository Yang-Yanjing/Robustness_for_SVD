static void mxf_write_cdci_desc(AVFormatContext *s, AVStream *st)
{
    mxf_write_cdci_common(s, st, mxf_cdci_descriptor_key, 0);
}

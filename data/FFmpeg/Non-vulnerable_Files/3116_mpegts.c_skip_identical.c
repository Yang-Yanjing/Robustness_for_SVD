static int skip_identical(const SectionHeader *h, MpegTSSectionFilter *tssf)
{
    if (h->version == tssf->last_ver && tssf->last_crc == tssf->crc)
        return 1;
    tssf->last_ver = h->version;
    tssf->last_crc = tssf->crc;
    return 0;
}

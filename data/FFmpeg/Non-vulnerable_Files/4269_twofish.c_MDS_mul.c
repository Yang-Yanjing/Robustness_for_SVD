static uint32_t MDS_mul(AVTWOFISH *cs, uint32_t X)
{
    return cs->MDS1[(X) & 0xff] ^ cs->MDS2[((X) >> 8) & 0xff] ^ cs->MDS3[((X) >> 16) & 0xff] ^ cs->MDS4[(X) >> 24];
}

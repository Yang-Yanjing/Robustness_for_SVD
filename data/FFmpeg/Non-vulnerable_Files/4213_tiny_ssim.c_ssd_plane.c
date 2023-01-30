uint64_t ssd_plane( const uint8_t *pix1, const uint8_t *pix2, int size )
{
    uint64_t ssd = 0;
    int i;
    for( i=0; i<size; i++ )
    {
        int d = pix1[i] - pix2[i];
        ssd += d*d;
    }
    return ssd;
}

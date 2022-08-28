static double ssd_to_psnr( uint64_t ssd, uint64_t denom )
{
    return -10*log((double)ssd/(denom*255*255))/log(10);
}

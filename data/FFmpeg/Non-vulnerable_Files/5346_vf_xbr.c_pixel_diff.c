AVFILTER_DEFINE_CLASS(xbr);
static uint32_t pixel_diff(uint32_t x, uint32_t y, const uint32_t *r2y)
{
#define YMASK 0xff0000
#define UMASK 0x00ff00
#define VMASK 0x0000ff
    uint32_t yuv1 = r2y[x & 0xffffff];
    uint32_t yuv2 = r2y[y & 0xffffff];
    return (abs((yuv1 & YMASK) - (yuv2 & YMASK)) >> 16) +
           (abs((yuv1 & UMASK) - (yuv2 & UMASK)) >>  8) +
           abs((yuv1 & VMASK) - (yuv2 & VMASK));
}

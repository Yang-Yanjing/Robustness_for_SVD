static av_always_inline int yuv_diff(uint32_t yuv1, uint32_t yuv2)
{
#define YMASK 0xff0000
#define UMASK 0x00ff00
#define VMASK 0x0000ff
    return abs((yuv1 & YMASK) - (yuv2 & YMASK)) > (48 << 16) ||
           abs((yuv1 & UMASK) - (yuv2 & UMASK)) > ( 7 <<  8) ||
           abs((yuv1 & VMASK) - (yuv2 & VMASK)) > ( 6 <<  0);
}

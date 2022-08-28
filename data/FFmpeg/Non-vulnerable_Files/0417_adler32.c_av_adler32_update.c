unsigned long av_adler32_update(unsigned long adler, const uint8_t * buf,
                                unsigned int len)
{
    unsigned long s1 = adler & 0xffff;
    unsigned long s2 = adler >> 16;
    while (len > 0) {
#if HAVE_FAST_64BIT && HAVE_FAST_UNALIGNED && !CONFIG_SMALL
        unsigned len2 = FFMIN((len-1) & ~7, 23*8);
        if (len2) {
            uint64_t a1= 0;
            uint64_t a2= 0;
            uint64_t b1= 0;
            uint64_t b2= 0;
            len -= len2;
            s2 += s1*len2;
            while (len2 >= 8) {
                uint64_t v = AV_RN64(buf);
                a2 += a1;
                b2 += b1;
                a1 +=  v    &0x00FF00FF00FF00FF;
                b1 += (v>>8)&0x00FF00FF00FF00FF;
                len2 -= 8;
                buf+=8;
            }
            
            
            
            
            s1 += ((a1+b1)*0x1000100010001)>>48;
            s2 += ((((a2&0xFFFF0000FFFF)+(b2&0xFFFF0000FFFF)+((a2>>16)&0xFFFF0000FFFF)+((b2>>16)&0xFFFF0000FFFF))*0x800000008)>>32)
#if HAVE_BIGENDIAN
                 + 2*((b1*0x1000200030004)>>48)
                 +   ((a1*0x1000100010001)>>48)
                 + 2*((a1*0x0000100020003)>>48);
#else
                 + 2*((a1*0x4000300020001)>>48)
                 +   ((b1*0x1000100010001)>>48)
                 + 2*((b1*0x3000200010000)>>48);
#endif
        }
#else
        while (len > 4  && s2 < (1U << 31)) {
            DO4(buf);
            len -= 4;
        }
#endif
        DO1(buf); len--;
        s1 %= BASE;
        s2 %= BASE;
    }
    return (s2 << 16) | s1;
}

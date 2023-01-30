static void sobel(int w, int h,
                       uint16_t *dst, int dst_linesize,
                         int8_t *dir, int dir_linesize,
                  const uint8_t *src, int src_linesize)
{
    int i, j;
    for (j = 1; j < h - 1; j++) {
        dst += dst_linesize;
        dir += dir_linesize;
        src += src_linesize;
        for (i = 1; i < w - 1; i++) {
            const int gx =
                -1*src[-src_linesize + i-1] + 1*src[-src_linesize + i+1]
                -2*src[                i-1] + 2*src[                i+1]
                -1*src[ src_linesize + i-1] + 1*src[ src_linesize + i+1];
            const int gy =
                -1*src[-src_linesize + i-1] + 1*src[ src_linesize + i-1]
                -2*src[-src_linesize + i  ] + 2*src[ src_linesize + i  ]
                -1*src[-src_linesize + i+1] + 1*src[ src_linesize + i+1];
            dst[i] = FFABS(gx) + FFABS(gy);
            dir[i] = get_rounded_direction(gx, gy);
        }
    }
}

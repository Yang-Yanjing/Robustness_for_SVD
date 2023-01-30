static av_always_inline void hqx_filter(const ThreadData *td, int jobnr, int nb_jobs, int n)
{
    int x, y;
    AVFrame *in = td->in, *out = td->out;
    const uint32_t *r2y = td->rgbtoyuv;
    const int height = in->height;
    const int width  = in->width;
    const int slice_start = (height *  jobnr   ) / nb_jobs;
    const int slice_end   = (height * (jobnr+1)) / nb_jobs;
    const int dst_linesize = out->linesize[0];
    const int src_linesize =  in->linesize[0];
    uint8_t       *dst = out->data[0] + slice_start * dst_linesize * n;
    const uint8_t *src =  in->data[0] + slice_start * src_linesize;
    const int dst32_linesize = dst_linesize >> 2;
    const int src32_linesize = src_linesize >> 2;
    for (y = slice_start; y < slice_end; y++) {
        const uint32_t *src32 = (const uint32_t *)src;
        uint32_t       *dst32 = (uint32_t *)dst;
        const int prevline = y > 0          ? -src32_linesize : 0;
        const int nextline = y < height - 1 ?  src32_linesize : 0;
        for (x = 0; x < width; x++) {
            const int prevcol = x > 0        ? -1 : 0;
            const int nextcol = x < width -1 ?  1 : 0;
            const uint32_t w[3*3] = {
                src32[prevcol + prevline], src32[prevline], src32[prevline + nextcol],
                src32[prevcol           ], src32[       0], src32[           nextcol],
                src32[prevcol + nextline], src32[nextline], src32[nextline + nextcol]
            };
            const uint32_t yuv1 = rgb2yuv(r2y, w[4]);
            const int pattern = (w[4] != w[0] ? (yuv_diff(yuv1, rgb2yuv(r2y, w[0]))) : 0)
                              | (w[4] != w[1] ? (yuv_diff(yuv1, rgb2yuv(r2y, w[1]))) : 0) << 1
                              | (w[4] != w[2] ? (yuv_diff(yuv1, rgb2yuv(r2y, w[2]))) : 0) << 2
                              | (w[4] != w[3] ? (yuv_diff(yuv1, rgb2yuv(r2y, w[3]))) : 0) << 3
                              | (w[4] != w[5] ? (yuv_diff(yuv1, rgb2yuv(r2y, w[5]))) : 0) << 4
                              | (w[4] != w[6] ? (yuv_diff(yuv1, rgb2yuv(r2y, w[6]))) : 0) << 5
                              | (w[4] != w[7] ? (yuv_diff(yuv1, rgb2yuv(r2y, w[7]))) : 0) << 6
                              | (w[4] != w[8] ? (yuv_diff(yuv1, rgb2yuv(r2y, w[8]))) : 0) << 7;
            if (n == 2) {
                dst32[dst32_linesize*0 + 0] = hq2x_interp_1x1(r2y, pattern, w, 0,1,2,3,4,5,6,7,8);  
                dst32[dst32_linesize*0 + 1] = hq2x_interp_1x1(r2y, pattern, w, 2,1,0,5,4,3,8,7,6);  
                dst32[dst32_linesize*1 + 0] = hq2x_interp_1x1(r2y, pattern, w, 6,7,8,3,4,5,0,1,2);  
                dst32[dst32_linesize*1 + 1] = hq2x_interp_1x1(r2y, pattern, w, 8,7,6,5,4,3,2,1,0);  
            } else if (n == 3) {
                hq3x_interp_2x1(dst32,                        dst32_linesize, r2y, pattern, w, 0,1, 0,1,2,3,4,5,6,7,8, 0);  
                hq3x_interp_2x1(dst32 + 1,                    dst32_linesize, r2y, pattern, w, 1,3, 2,5,8,1,4,7,0,3,6, 1);  
                hq3x_interp_2x1(dst32 + 1*dst32_linesize,     dst32_linesize, r2y, pattern, w, 2,0, 6,3,0,7,4,1,8,5,2, 1);  
                hq3x_interp_2x1(dst32 + 1*dst32_linesize + 1, dst32_linesize, r2y, pattern, w, 3,2, 8,7,6,5,4,3,2,1,0, 0);  
                dst32[dst32_linesize + 1] = w[4];                                                                           
            } else if (n == 4) {
                hq4x_interp_2x2(dst32,                        dst32_linesize, r2y, pattern, w, 0,1,2,3, 0,1,2,3,4,5,6,7,8); 
                hq4x_interp_2x2(dst32 + 2,                    dst32_linesize, r2y, pattern, w, 1,0,3,2, 2,1,0,5,4,3,8,7,6); 
                hq4x_interp_2x2(dst32 + 2*dst32_linesize,     dst32_linesize, r2y, pattern, w, 2,3,0,1, 6,7,8,3,4,5,0,1,2); 
                hq4x_interp_2x2(dst32 + 2*dst32_linesize + 2, dst32_linesize, r2y, pattern, w, 3,2,1,0, 8,7,6,5,4,3,2,1,0); 
            } else {
                av_assert0(0);
            }
            src32 += 1;
            dst32 += n;
        }
        src += src_linesize;
        dst += dst_linesize * n;
    }
}

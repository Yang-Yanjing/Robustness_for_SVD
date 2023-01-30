static void blur_image(int ***mask,
                       const uint8_t *src_data,  int src_linesize,
                             uint8_t *dst_data,  int dst_linesize,
                       const uint8_t *mask_data, int mask_linesize,
                       int w, int h, int direct,
                       FFBoundingBox *bbox)
{
    int x, y;
    uint8_t *dst_line;
    const uint8_t *src_line;
    if (!direct)
        av_image_copy_plane(dst_data, dst_linesize, src_data, src_linesize, w, h);
    for (y = bbox->y1; y <= bbox->y2; y++) {
        src_line = src_data + src_linesize * y;
        dst_line = dst_data + dst_linesize * y;
        for (x = bbox->x1; x <= bbox->x2; x++) {
             if (mask_data[y * mask_linesize + x]) {
                
                 dst_line[x] = blur_pixel(mask,
                                          mask_data, mask_linesize,
                                          dst_data, dst_linesize,
                                          w, h, x, y);
            } else {
                
                if (!direct)
                    dst_line[x] = src_line[x];
            }
        }
    }
}

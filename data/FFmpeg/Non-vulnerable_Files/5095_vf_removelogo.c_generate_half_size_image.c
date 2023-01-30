static void generate_half_size_image(const uint8_t *src_data, int src_linesize,
                                     uint8_t *dst_data, int dst_linesize,
                                     int src_w, int src_h,
                                     int *max_mask_size)
{
    int x, y;
    

    for (y = 0; y < src_h/2; y++) {
        for (x = 0; x < src_w/2; x++) {
            

            dst_data[(y * dst_linesize) + x] =
                src_data[((y << 1) * src_linesize) + (x << 1)] ||
                src_data[((y << 1) * src_linesize) + (x << 1) + 1] ||
                src_data[(((y << 1) + 1) * src_linesize) + (x << 1)] ||
                src_data[(((y << 1) + 1) * src_linesize) + (x << 1) + 1];
            dst_data[(y * dst_linesize) + x] = FFMIN(1, dst_data[(y * dst_linesize) + x]);
        }
    }
    convert_mask_to_strength_mask(dst_data, dst_linesize,
                                  src_w/2, src_h/2, 0, max_mask_size);
}

static void convert_mask_to_strength_mask(uint8_t *data, int linesize,
                                          int w, int h, int min_val,
                                          int *max_mask_size)
{
    int x, y;
    

    int current_pass = 0;
    
    for (y = 0; y < h; y++)
        for (x = 0; x < w; x++)
            data[y*linesize + x] = data[y*linesize + x] > min_val;
    





    while (1) {
        
        int has_anything_changed = 0;
        uint8_t *current_pixel0 = data + 1 + linesize, *current_pixel;
        current_pass++;
        for (y = 1; y < h-1; y++) {
            current_pixel = current_pixel0;
            for (x = 1; x < w-1; x++) {
                









                if ( *current_pixel      >= current_pass &&
                    *(current_pixel + 1) >= current_pass &&
                    *(current_pixel - 1) >= current_pass &&
                    *(current_pixel + linesize) >= current_pass &&
                    *(current_pixel - linesize) >= current_pass) {
                    


                    (*current_pixel)++;
                    has_anything_changed = 1;
                }
                current_pixel++;
            }
            current_pixel0 += linesize;
        }
        if (!has_anything_changed)
            break;
    }
    

    for (y = 1; y < h - 1; y++)
        for (x = 1; x < w - 1; x++)
            data[(y * linesize) + x] = apply_mask_fudge_factor(data[(y * linesize) + x]);
    

    

    *max_mask_size = apply_mask_fudge_factor(current_pass + 1);
}

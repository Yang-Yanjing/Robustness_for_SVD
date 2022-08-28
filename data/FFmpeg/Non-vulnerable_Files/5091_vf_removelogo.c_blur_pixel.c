static unsigned int blur_pixel(int ***mask,
                               const uint8_t *mask_data, int mask_linesize,
                               uint8_t       *image_data, int image_linesize,
                               int w, int h, int x, int y)
{
    

    int mask_size;
    int start_posx, start_posy, end_posx, end_posy;
    int i, j;
    unsigned int accumulator = 0, divisor = 0;
    
    const uint8_t *image_read_position;
    
    const uint8_t *mask_read_position;
    
    mask_size  = mask_data[y * mask_linesize + x];
    start_posx = FFMAX(0, x - mask_size);
    start_posy = FFMAX(0, y - mask_size);
    end_posx   = FFMIN(w - 1, x + mask_size);
    end_posy   = FFMIN(h - 1, y + mask_size);
    image_read_position = image_data + image_linesize * start_posy + start_posx;
    mask_read_position  = mask_data  + mask_linesize  * start_posy + start_posx;
    for (j = start_posy; j <= end_posy; j++) {
        for (i = start_posx; i <= end_posx; i++) {
            

            if (!(*mask_read_position) && mask[mask_size][i - start_posx][j - start_posy]) {
                accumulator += *image_read_position;
                divisor++;
            }
            image_read_position++;
            mask_read_position++;
        }
        image_read_position += (image_linesize - ((end_posx + 1) - start_posx));
        mask_read_position  += (mask_linesize - ((end_posx + 1) - start_posx));
    }
    


    return divisor == 0 ? 255:
        (accumulator + (divisor / 2)) / divisor;  
}

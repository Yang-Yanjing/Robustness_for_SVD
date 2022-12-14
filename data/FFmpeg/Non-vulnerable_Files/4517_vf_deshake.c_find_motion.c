static void find_motion(DeshakeContext *deshake, uint8_t *src1, uint8_t *src2,
                        int width, int height, int stride, Transform *t)
{
    int x, y;
    IntMotionVector mv = {0, 0};
    int count_max_value = 0;
    int contrast;
    int pos;
    int center_x = 0, center_y = 0;
    double p_x, p_y;
    av_fast_malloc(&deshake->angles, &deshake->angles_size, width * height / (16 * deshake->blocksize) * sizeof(*deshake->angles));
    
    for (x = 0; x < deshake->rx * 2 + 1; x++) {
        for (y = 0; y < deshake->ry * 2 + 1; y++) {
            deshake->counts[x][y] = 0;
        }
    }
    pos = 0;
    
    for (y = deshake->ry; y < height - deshake->ry - (deshake->blocksize * 2); y += deshake->blocksize * 2) {
        
        for (x = deshake->rx; x < width - deshake->rx - 16; x += 16) {
            
            
            contrast = block_contrast(src2, x, y, stride, deshake->blocksize);
            if (contrast > deshake->contrast) {
                
                find_block_motion(deshake, src1, src2, x, y, stride, &mv);
                if (mv.x != -1 && mv.y != -1) {
                    deshake->counts[mv.x + deshake->rx][mv.y + deshake->ry] += 1;
                    if (x > deshake->rx && y > deshake->ry)
                        deshake->angles[pos++] = block_angle(x, y, 0, 0, &mv);
                    center_x += mv.x;
                    center_y += mv.y;
                }
            }
        }
    }
    if (pos) {
         center_x /= pos;
         center_y /= pos;
         t->angle = clean_mean(deshake->angles, pos);
         if (t->angle < 0.001)
              t->angle = 0;
    } else {
         t->angle = 0;
    }
    
    for (y = deshake->ry * 2; y >= 0; y--) {
        for (x = 0; x < deshake->rx * 2 + 1; x++) {
            
            if (deshake->counts[x][y] > count_max_value) {
                t->vec.x = x - deshake->rx;
                t->vec.y = y - deshake->ry;
                count_max_value = deshake->counts[x][y];
            }
        }
        
    }
    p_x = (center_x - width / 2.0);
    p_y = (center_y - height / 2.0);
    t->vec.x += (cos(t->angle)-1)*p_x  - sin(t->angle)*p_y;
    t->vec.y += sin(t->angle)*p_x  + (cos(t->angle)-1)*p_y;
    
    t->vec.x = av_clipf(t->vec.x, -deshake->rx * 2, deshake->rx * 2);
    t->vec.y = av_clipf(t->vec.y, -deshake->ry * 2, deshake->ry * 2);
    t->angle = av_clipf(t->angle, -0.1, 0.1);
    
}

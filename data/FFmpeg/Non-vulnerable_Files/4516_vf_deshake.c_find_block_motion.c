static void find_block_motion(DeshakeContext *deshake, uint8_t *src1,
                              uint8_t *src2, int cx, int cy, int stride,
                              IntMotionVector *mv)
{
    int x, y;
    int diff;
    int smallest = INT_MAX;
    int tmp, tmp2;
    #define CMP(i, j) deshake->sad(src1 + cy  * stride + cx,  stride,\
                                   src2 + (j) * stride + (i), stride)
    if (deshake->search == EXHAUSTIVE) {
        
        for (y = -deshake->ry; y <= deshake->ry; y++) {
            for (x = -deshake->rx; x <= deshake->rx; x++) {
                diff = CMP(cx - x, cy - y);
                if (diff < smallest) {
                    smallest = diff;
                    mv->x = x;
                    mv->y = y;
                }
            }
        }
    } else if (deshake->search == SMART_EXHAUSTIVE) {
        
        for (y = -deshake->ry + 1; y < deshake->ry; y += 2) {
            for (x = -deshake->rx + 1; x < deshake->rx; x += 2) {
                diff = CMP(cx - x, cy - y);
                if (diff < smallest) {
                    smallest = diff;
                    mv->x = x;
                    mv->y = y;
                }
            }
        }
        
        tmp = mv->x;
        tmp2 = mv->y;
        for (y = tmp2 - 1; y <= tmp2 + 1; y++) {
            for (x = tmp - 1; x <= tmp + 1; x++) {
                if (x == tmp && y == tmp2)
                    continue;
                diff = CMP(cx - x, cy - y);
                if (diff < smallest) {
                    smallest = diff;
                    mv->x = x;
                    mv->y = y;
                }
            }
        }
    }
    if (smallest > 512) {
        mv->x = -1;
        mv->y = -1;
    }
    emms_c();
    
    
}

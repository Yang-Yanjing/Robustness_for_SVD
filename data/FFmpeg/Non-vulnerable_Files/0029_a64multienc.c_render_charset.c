static void render_charset(AVCodecContext *avctx, uint8_t *charset,
                           uint8_t *colrammap)
{
    A64Context *c = avctx->priv_data;
    uint8_t row1, row2;
    int charpos, x, y;
    int a, b;
    uint8_t pix;
    int lowdiff, highdiff;
    int *best_cb = c->mc_best_cb;
    static uint8_t index1[256];
    static uint8_t index2[256];
    static uint8_t dither[256];
    int i;
    int distance;
    
    i = 0;
    for (a=0; a < 256; a++) {
        if(i < c->mc_pal_size -1 && a == c->mc_luma_vals[i + 1]) {
            distance = c->mc_luma_vals[i + 1] - c->mc_luma_vals[i];
            for(b = 0; b <= distance; b++) {
                  dither[c->mc_luma_vals[i] + b] = b * (DITHERSTEPS - 1) / distance;
            }
            i++;
        }
        if(i >= c->mc_pal_size - 1) dither[a] = 0;
        index1[a] = i;
        index2[a] = FFMIN(i + 1, c->mc_pal_size - 1);
    }
    
    for (charpos = 0; charpos < CHARSET_CHARS; charpos++) {
        lowdiff  = 0;
        highdiff = 0;
        for (y = 0; y < 8; y++) {
            row1 = 0; row2 = 0;
            for (x = 0; x < 4; x++) {
                pix = best_cb[y * 4 + x];
                
                if (index1[pix] >= 3)
                    highdiff += pix - c->mc_luma_vals[3];
                if (index1[pix] < 1)
                    lowdiff += c->mc_luma_vals[1] - pix;
                row1 <<= 2;
                if (INTERLACED) {
                    row2 <<= 2;
                    if (interlaced_dither_patterns[dither[pix]][(y & 3) * 2 + 0][x & 3])
                        row1 |= 3-(index2[pix] & 3);
                    else
                        row1 |= 3-(index1[pix] & 3);
                    if (interlaced_dither_patterns[dither[pix]][(y & 3) * 2 + 1][x & 3])
                        row2 |= 3-(index2[pix] & 3);
                    else
                        row2 |= 3-(index1[pix] & 3);
                }
                else {
                    if (multi_dither_patterns[dither[pix]][(y & 3)][x & 3])
                        row1 |= 3-(index2[pix] & 3);
                    else
                        row1 |= 3-(index1[pix] & 3);
                }
            }
            charset[y+0x000] = row1;
            if (INTERLACED) charset[y+0x800] = row2;
        }
        
        if (highdiff > 0 && lowdiff > 0 && c->mc_use_5col) {
            if (lowdiff > highdiff) {
                for (x = 0; x < 32; x++)
                    best_cb[x] = FFMIN(c->mc_luma_vals[3], best_cb[x]);
            } else {
                for (x = 0; x < 32; x++)
                    best_cb[x] = FFMAX(c->mc_luma_vals[1], best_cb[x]);
            }
            charpos--;          
        
        } else {
            
            best_cb += 32;
            charset += 8;
            
            colrammap[charpos] = (highdiff > 0);
        }
    }
}

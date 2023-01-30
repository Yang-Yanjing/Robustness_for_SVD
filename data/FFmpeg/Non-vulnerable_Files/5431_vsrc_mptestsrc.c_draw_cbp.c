static void draw_cbp(uint8_t *dst[3], int dst_linesize[3], int cbp, int amp, int dc)
{
    if (cbp&1)  draw_basis(dst[0]                    , dst_linesize[0], amp, 1, dc);
    if (cbp&2)  draw_basis(dst[0]+8                  , dst_linesize[0], amp, 1, dc);
    if (cbp&4)  draw_basis(dst[0]+  8*dst_linesize[0], dst_linesize[0], amp, 1, dc);
    if (cbp&8)  draw_basis(dst[0]+8+8*dst_linesize[0], dst_linesize[0], amp, 1, dc);
    if (cbp&16) draw_basis(dst[1]                    , dst_linesize[1], amp, 1, dc);
    if (cbp&32) draw_basis(dst[2]                    , dst_linesize[2], amp, 1, dc);
}

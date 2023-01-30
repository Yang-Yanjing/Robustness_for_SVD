};
static scte27_color_t bs_read_color(bs_t *bs)
{
    scte27_color_t color;
    /* XXX it's unclear if a value of 0 in Y/U/V means a transparent pixel */
    color.y     = bs_read(bs, 5) << 3;
    color.alpha = bs_read1(bs) ? 0xff : 0x80;
    color.v     = bs_read(bs, 5) << 3;
    color.u     = bs_read(bs, 5) << 3;
    return color;
}

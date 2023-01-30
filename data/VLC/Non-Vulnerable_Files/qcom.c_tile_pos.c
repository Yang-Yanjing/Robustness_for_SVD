/* get frame tile coordinate. XXX: nothing to be understood here, don't try. */
static size_t tile_pos(size_t x, size_t y, size_t w, size_t h)
{
    size_t flim = x + (y & ~1) * w;
    if (y & 1) {
        flim += (x & ~3) + 2;
    } else if ((h & 1) == 0 || y != (h - 1)) {
        flim += (x + 2) & ~3;
    }
    return flim;
}

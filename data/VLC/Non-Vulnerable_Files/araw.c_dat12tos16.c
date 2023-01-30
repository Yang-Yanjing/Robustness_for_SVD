}
static int16_t dat12tos16( uint16_t y )
{
    static const uint16_t diff[16] = {
       0x0000, 0x0000, 0x0100, 0x0200, 0x0300, 0x0400, 0x0500, 0x0600,
       0x0A00, 0x0B00, 0x0C00, 0x0D00, 0x0E00, 0x0F00, 0x1000, 0x1000 };
    static const uint8_t shift[16] = {
        0, 0, 1, 2, 3, 4, 5, 6, 6, 5, 4, 3, 2, 1, 0, 0 };
    int d = y >> 8;
    return (y - diff[d]) << shift[d];
}

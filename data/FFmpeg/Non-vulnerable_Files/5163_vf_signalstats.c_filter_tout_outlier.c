static int filter_tout_outlier(uint8_t x, uint8_t y, uint8_t z)
{
    return ((abs(x - y) + abs (z - y)) / 2) - abs(z - x) > 4; 
}

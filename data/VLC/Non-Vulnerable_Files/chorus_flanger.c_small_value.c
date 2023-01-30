 */
static inline float small_value()
{
    /* allows for 2^-24, should be enough for 24-bit DACs at least */
    return ( 1.0 / 16777216.0 );
}

/* Utility function that reads a big endian double from the input buffer. */
static inline double GetDBLBE( const uint8_t *p )
{
    union
    {
        uint64_t uint64;
        double dbl;
    } u_64;
    u_64.uint64 = GetQWBE( p );
    return u_64.dbl;
}

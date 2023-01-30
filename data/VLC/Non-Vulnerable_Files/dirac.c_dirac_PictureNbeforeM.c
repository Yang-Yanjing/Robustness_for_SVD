static int dirac_numRefs( uint8_t u_parse_code ) { return 0x3 & u_parse_code; }
static inline bool dirac_PictureNbeforeM( uint32_t u_n, uint32_t u_m )
{
    /* specified as: u_n occurs before u_m if:
     *   (u_m - u_n) mod (1<<32) < D */
    return (uint32_t)(u_m - u_n) < (1u<<31);
}

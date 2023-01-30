 *****************************************************************************/
static inline void GetDoubleLE( double *pd, void *src )
{
    /* FIXME works only if sizeof(double) == 8 */
#ifdef WORDS_BIGENDIAN
    uint8_t *p = (uint8_t*)pd, *q = (uint8_t*)src;
    int i;
    for( i = 0; i < 8; i++ )
        p[i] = q[7-i];
#else
    memcpy( pd, src, 8 );
#endif
}

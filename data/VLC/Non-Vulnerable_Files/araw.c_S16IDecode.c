}
static void S16IDecode( void *out, const uint8_t *in, unsigned samples )
{
    swab( in, out, samples * 2 );
}

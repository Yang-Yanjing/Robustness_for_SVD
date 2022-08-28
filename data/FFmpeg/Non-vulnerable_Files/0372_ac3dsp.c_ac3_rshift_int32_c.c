static void ac3_rshift_int32_c(int32_t *src, unsigned int len,
                               unsigned int shift)
{
    do {
        *src++ >>= shift;
        *src++ >>= shift;
        *src++ >>= shift;
        *src++ >>= shift;
        *src++ >>= shift;
        *src++ >>= shift;
        *src++ >>= shift;
        *src++ >>= shift;
        len -= 8;
    } while (len > 0);
}

}
static int string_to_IV(char *string_hexa, uint8_t iv[AES_BLOCK_SIZE])
{
    unsigned long long iv_hi, iv_lo;
    char *end = NULL;
    if (*string_hexa++ != '0')
        return VLC_EGENERIC;
    if (*string_hexa != 'x' && *string_hexa != 'X')
        return VLC_EGENERIC;
    string_hexa++;
    size_t len = strlen(string_hexa);
    if (len <= 16) {
        iv_hi = 0;
        iv_lo = strtoull(string_hexa, &end, 16);
        if (*end)
            return VLC_EGENERIC;
    } else {
        iv_lo = strtoull(&string_hexa[len-16], &end, 16);
        if (*end)
            return VLC_EGENERIC;
        string_hexa[len-16] = '\0';
        iv_hi = strtoull(string_hexa, &end, 16);
        if (*end)
            return VLC_EGENERIC;
    }
    for (int i = 7; i >= 0 ; --i) {
        iv[  i] = iv_hi & 0xff;
        iv[8+i] = iv_lo & 0xff;
        iv_hi >>= 8;
        iv_lo >>= 8;
    }
    return VLC_SUCCESS;
}

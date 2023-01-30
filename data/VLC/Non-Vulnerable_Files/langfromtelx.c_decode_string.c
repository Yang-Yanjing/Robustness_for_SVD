}
static void decode_string( char *res, uint8_t *packet, int len )
{
    for ( int i = 0; i < len; i++ )
        res[i] = bytereverse( packet[i] ) & 0x7f;
    res[len] = '\0';
}

}
static void vnc_encrypt_bytes( unsigned char *bytes, char *passwd )
{
    unsigned char key[8];
    unsigned int i;
    for (i = 0; i < 8; i++)
        key[i] = i < strlen( passwd ) ? passwd[i] : '\0';
    gcry_cipher_hd_t ctx;
    gcry_cipher_open( &ctx, GCRY_CIPHER_DES, GCRY_CIPHER_MODE_ECB,0);
    /* reverse bits of the key */
    for( i = 0 ; i < 8 ; i ++ )
        key[i] =
            (key[i] >> 7) +
            (((key[i] >> 6) & 0x01 ) << 1 ) +
            (((key[i] >> 5) & 0x01 ) << 2 ) +
            (((key[i] >> 4) & 0x01 ) << 3 ) +
            (((key[i] >> 3) & 0x01 ) << 4 ) +
            (((key[i] >> 2) & 0x01 ) << 5 ) +
            (((key[i] >> 1) & 0x01 ) << 6 ) +
            ((key[i] & 0x01) << 7 );
    gcry_cipher_setkey( ctx, key, 8 );
    gcry_cipher_encrypt( ctx, bytes, CHALLENGESIZE, bytes, CHALLENGESIZE );
    gcry_cipher_close( ctx );
}

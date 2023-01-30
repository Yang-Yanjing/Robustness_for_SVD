 */
static int AddOaepPadding( vlc_object_t *p_this,
                           unsigned char *EM, const size_t emLenWithPrefix,
                           const unsigned char *M, const size_t mLen,
                           const unsigned char *P, const size_t pLen )
{
    const int Hash = GCRY_MD_SHA1;
    const unsigned int hLen = gcry_md_get_algo_dlen( Hash );
    unsigned char *seed = NULL;
    unsigned char *DB = NULL;
    unsigned char *dbMask = NULL;
    unsigned char *seedMask = NULL;
    size_t emLen;
    size_t psLen;
    size_t i;
    int i_err = VLC_SUCCESS;
    /* Space for 0x00 prefix in EM. */
    emLen = emLenWithPrefix - 1;
    /* Step 2:
     * If ||M|| > emLen-2hLen-1 then output "message too long" and stop.
     */
    if ( mLen > (emLen - (2 * hLen) - 1) )
    {
        msg_Err( p_this , "Message too long" );
        goto error;
    }
    /* Step 3:
     * Generate an octet string PS consisting of emLen-||M||-2hLen-1 zero
     * octets. The length of PS may be 0.
     */
    psLen = emLen - mLen - (2 * hLen) - 1;
    /*
     * Step 5:
     * Concatenate pHash, PS, the message M, and other padding to form a data
     * block DB as: DB = pHash || PS || 01 || M
     */
    DB = calloc( 1, hLen + psLen + 1 + mLen );
    dbMask = calloc( 1, emLen - hLen );
    seedMask = calloc( 1, hLen );
    if ( DB == NULL || dbMask == NULL || seedMask == NULL )
    {
        i_err = VLC_ENOMEM;
        goto error;
    }
    /* Step 4:
     * Let pHash = Hash(P), an octet string of length hLen.
     */
    gcry_md_hash_buffer( Hash, DB, P, pLen );
    /* Step 3:
     * Generate an octet string PS consisting of emLen-||M||-2hLen-1 zero
     * octets. The length of PS may be 0.
     */
    memset( DB + hLen, 0, psLen );
    /* Step 5:
     * Concatenate pHash, PS, the message M, and other padding to form a data
     * block DB as: DB = pHash || PS || 01 || M
     */
    DB[hLen + psLen] = 0x01;
    memcpy( DB + hLen + psLen + 1, M, mLen );
    /* Step 6:
     * Generate a random octet string seed of length hLen
     */
    seed = gcry_random_bytes( hLen, GCRY_STRONG_RANDOM );
    if ( seed == NULL )
    {
        i_err = VLC_ENOMEM;
        goto error;
    }
    /* Step 7:
     * Let dbMask = MGF(seed, emLen-hLen).
     */
    i_err = MGF1( p_this, dbMask, emLen - hLen, seed, hLen, Hash );
    if ( i_err != VLC_SUCCESS )
        goto error;
    /* Step 8:
     * Let maskedDB = DB \xor dbMask.
     */
    for ( i = 0; i < (emLen - hLen); ++i )
        DB[i] ^= dbMask[i];
    /* Step 9:
     * Let seedMask = MGF(maskedDB, hLen).
     */
    i_err = MGF1( p_this, seedMask, hLen, DB, emLen - hLen, Hash );
    if ( i_err != VLC_SUCCESS )
        goto error;
    /* Step 10:
     * Let maskedSeed = seed \xor seedMask.
     */
    for ( i = 0; i < hLen; ++i )
        seed[i] ^= seedMask[i];
    /* Step 11:
     * Let EM = maskedSeed || maskedDB.
     */
    assert( (1 + hLen + (hLen + psLen + 1 + mLen)) == emLenWithPrefix );
    EM[0] = 0x00;
    memcpy( EM + 1, seed, hLen );
    memcpy( EM + 1 + hLen, DB, hLen + psLen + 1 + mLen );
    /* Step 12:
     * Output EM.
     */
error:
    free( DB );
    free( dbMask );
    free( seedMask );
    free( seed );
    return i_err;
}

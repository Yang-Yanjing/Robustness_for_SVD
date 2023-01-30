}
static void csa_BlockCypher( uint8_t kk[57], uint8_t bd[8], uint8_t ib[8] )
{
    int i;
    int perm_out;
    int R[9];
    int next_R1;
    for( i = 0; i < 8; i++ )
    {
        R[i+1] = bd[i];
    }
    // loop over kk[1]..kk[56]
    for( i = 1; i <= 56; i++ )
    {
        const int sbox_out = block_sbox[ kk[i]^R[8] ];
        perm_out = block_perm[sbox_out];
        next_R1 = R[2];
        R[2] = R[3] ^ R[1];
        R[3] = R[4] ^ R[1];
        R[4] = R[5] ^ R[1];
        R[5] = R[6];
        R[6] = R[7] ^ perm_out;
        R[7] = R[8];
        R[8] = R[1] ^ sbox_out;
        R[1] = next_R1;
    }
    for( i = 0; i < 8; i++ )
    {
        ib[i] = R[i+1];
    }
}

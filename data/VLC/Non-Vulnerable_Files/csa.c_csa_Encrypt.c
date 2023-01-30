 *****************************************************************************/
void csa_Encrypt( csa_t *c, uint8_t *pkt, int i_pkt_size )
{
    uint8_t *ck;
    uint8_t *kk;
    int i, j;
    int i_hdr = 4; /* hdr len */
    uint8_t  ib[184/8+2][8], stream[8], block[8];
    int n, i_residue;
    /* set transport scrambling control */
    pkt[3] |= 0x80;
    if( c->use_odd )
    {
        pkt[3] |= 0x40;
        ck = c->o_ck;
        kk = c->o_kk;
    }
    else
    {
        ck = c->e_ck;
        kk = c->e_kk;
    }
    /* hdr len */
    i_hdr = 4;
    if( pkt[3]&0x20 )
    {
        /* skip adaption field */
        i_hdr += pkt[4] + 1;
    }
    n = (i_pkt_size - i_hdr) / 8;
    i_residue = (i_pkt_size - i_hdr) % 8;
    if( n <= 0 )
    {
        pkt[3] &= 0x3f;
        return;
    }
    /* */
    for( i = 0; i < 8; i++ )
    {
        ib[n+1][i] = 0;
    }
    for( i = n; i  > 0; i-- )
    {
        for( j = 0; j < 8; j++ )
        {
            block[j] = pkt[i_hdr+8*(i-1)+j] ^ib[i+1][j];
        }
        csa_BlockCypher( kk, block, ib[i] );
    }
    /* init csa state */
    csa_StreamCypher( c, 1, ck, ib[1], stream );
    for( i = 0; i < 8; i++ )
    {
        pkt[i_hdr+i] = ib[1][i];
    }
    for( i = 2; i < n+1; i++ )
    {
        csa_StreamCypher( c, 0, ck, NULL, stream );
        for( j = 0; j < 8; j++ )
        {
            pkt[i_hdr+8*(i-1)+j] = ib[i][j] ^ stream[j];
        }
    }
    if( i_residue > 0 )
    {
        csa_StreamCypher( c, 0, ck, NULL, stream );
        for( j = 0; j < i_residue; j++ )
        {
            pkt[i_pkt_size - i_residue + j] ^= stream[j];
        }
    }
}

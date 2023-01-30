static const int sbox7[0x20] = {0,3,2,2,3,0,0,1, 3,0,1,3,1,2,2,1, 1,0,3,3,0,1,1,2, 2,3,1,0,2,3,0,2};
static void csa_StreamCypher( csa_t *c, int b_init, uint8_t *ck, uint8_t *sb, uint8_t *cb )
{
    int i,j, k;
    int extra_B;
    int s1,s2,s3,s4,s5,s6,s7;
    int next_A1;
    int next_B1;
    int next_E;
    if( b_init )
    {
        // load first 32 bits of CK into A[1]..A[8]
        // load last  32 bits of CK into B[1]..B[8]
        // all other regs = 0
        for( i = 0; i < 4; i++ )
        {
            c->A[1+2*i+0] = ( ck[i] >> 4 )&0x0f;
            c->A[1+2*i+1] = ( ck[i] >> 0 )&0x0f;
            c->B[1+2*i+0] = ( ck[4+i] >> 4 )&0x0f;
            c->B[1+2*i+1] = ( ck[4+i] >> 0 )&0x0f;
        }
        c->A[9] = c->A[10] = 0;
        c->B[9] = c->B[10] = 0;
        c->X = c->Y = c->Z = 0;
        c->D = c->E = c->F = 0;
        c->p = c->q = c->r = 0;
    }
    // 8 bytes per operation
    for( i = 0; i < 8; i++ )
    {
        int op = 0;
        int in1 = 0;    /* gcc warn */
        int in2 = 0;
        if( b_init )
        {
            in1 = ( sb[i] >> 4 )&0x0f;
            in2 = ( sb[i] >> 0 )&0x0f;
        }
        // 2 bits per iteration
        for( j = 0; j < 4; j++ )
        {
            // from A[1]..A[10], 35 bits are selected as inputs to 7 s-boxes
            // 5 bits input per s-box, 2 bits output per s-box
            s1 = sbox1[ (((c->A[4]>>0)&1)<<4) | (((c->A[1]>>2)&1)<<3) | (((c->A[6]>>1)&1)<<2) | (((c->A[7]>>3)&1)<<1) | (((c->A[9]>>0)&1)<<0) ];
            s2 = sbox2[ (((c->A[2]>>1)&1)<<4) | (((c->A[3]>>2)&1)<<3) | (((c->A[6]>>3)&1)<<2) | (((c->A[7]>>0)&1)<<1) | (((c->A[9]>>1)&1)<<0) ];
            s3 = sbox3[ (((c->A[1]>>3)&1)<<4) | (((c->A[2]>>0)&1)<<3) | (((c->A[5]>>1)&1)<<2) | (((c->A[5]>>3)&1)<<1) | (((c->A[6]>>2)&1)<<0) ];
            s4 = sbox4[ (((c->A[3]>>3)&1)<<4) | (((c->A[1]>>1)&1)<<3) | (((c->A[2]>>3)&1)<<2) | (((c->A[4]>>2)&1)<<1) | (((c->A[8]>>0)&1)<<0) ];
            s5 = sbox5[ (((c->A[5]>>2)&1)<<4) | (((c->A[4]>>3)&1)<<3) | (((c->A[6]>>0)&1)<<2) | (((c->A[8]>>1)&1)<<1) | (((c->A[9]>>2)&1)<<0) ];
            s6 = sbox6[ (((c->A[3]>>1)&1)<<4) | (((c->A[4]>>1)&1)<<3) | (((c->A[5]>>0)&1)<<2) | (((c->A[7]>>2)&1)<<1) | (((c->A[9]>>3)&1)<<0) ];
            s7 = sbox7[ (((c->A[2]>>2)&1)<<4) | (((c->A[3]>>0)&1)<<3) | (((c->A[7]>>1)&1)<<2) | (((c->A[8]>>2)&1)<<1) | (((c->A[8]>>3)&1)<<0) ];
            /* use 4x4 xor to produce extra nibble for T3 */
            extra_B = ( ((c->B[3]&1)<<3) ^ ((c->B[6]&2)<<2) ^ ((c->B[7]&4)<<1) ^ ((c->B[9]&8)>>0) ) |
                      ( ((c->B[6]&1)<<2) ^ ((c->B[8]&2)<<1) ^ ((c->B[3]&8)>>1) ^ ((c->B[4]&4)>>0) ) |
                      ( ((c->B[5]&8)>>2) ^ ((c->B[8]&4)>>1) ^ ((c->B[4]&1)<<1) ^ ((c->B[5]&2)>>0) ) |
                      ( ((c->B[9]&4)>>2) ^ ((c->B[6]&8)>>3) ^ ((c->B[3]&2)>>1) ^ ((c->B[8]&1)>>0) ) ;
            // T1 = xor all inputs
            // in1,in2, D are only used in T1 during initialisation, not generation
            next_A1 = c->A[10] ^ c->X;
            if( b_init ) next_A1 = next_A1 ^ c->D ^ ((j % 2) ? in2 : in1);
            // T2 =  xor all inputs
            // in1,in2 are only used in T1 during initialisation, not generation
            // if p=0, use this, if p=1, rotate the result left
            next_B1 = c->B[7] ^ c->B[10] ^ c->Y;
            if( b_init) next_B1 = next_B1 ^ ((j % 2) ? in1 : in2);
            // if p=1, rotate left
            if( c->p ) next_B1 = ( (next_B1 << 1) | ((next_B1 >> 3) & 1) ) & 0xf;
            // T3 = xor all inputs
            c->D = c->E ^ c->Z ^ extra_B;
            // T4 = sum, carry of Z + E + r
            next_E = c->F;
            if( c->q )
            {
                c->F = c->Z + c->E + c->r;
                // r is the carry
                c->r = (c->F >> 4) & 1;
                c->F = c->F & 0x0f;
            }
            else
            {
                c->F = c->E;
            }
            c->E = next_E;
            for( k = 10; k > 1; k-- )
            {
                c->A[k] = c->A[k-1];
                c->B[k] = c->B[k-1];
            }
            c->A[1] = next_A1;
            c->B[1] = next_B1;
            c->X = ((s4&1)<<3) | ((s3&1)<<2) | (s2&2) | ((s1&2)>>1);
            c->Y = ((s6&1)<<3) | ((s5&1)<<2) | (s4&2) | ((s3&2)>>1);
            c->Z = ((s2&1)<<3) | ((s1&1)<<2) | (s6&2) | ((s5&2)>>1);
            c->p = (s7&2)>>1;
            c->q = (s7&1);
            // require 4 loops per output byte
            // 2 output bits are a function of the 4 bits of D
            // xor 2 by 2
            op = (op << 2)^ ( (((c->D^(c->D>>1))>>1)&2) | ((c->D^(c->D>>1))&1) );
        }
        // return input data during init
        cb[i] = b_init ? sb[i] : op;
    }
}

static void precomputeMDS(AVTWOFISH *cs)
{
    uint8_t y[4];
    int i;
    for (i = 0; i < 256; i++) {
        y[0] = y[1] = y[2] = y[3] = i;
    tf_h0(y, cs->S, cs->ksize);
        cs->MDS1[i] = ((uint32_t)y[0]) ^ ((uint32_t)MD1[y[0]] << 8) ^ ((uint32_t)MD2[y[0]] << 16) ^ ((uint32_t)MD2[y[0]] << 24);
        cs->MDS2[i] = ((uint32_t)MD2[y[1]]) ^ ((uint32_t)MD2[y[1]] << 8) ^ ((uint32_t)MD1[y[1]] << 16) ^ ((uint32_t)y[1] << 24);
        cs->MDS3[i] = ((uint32_t)MD1[y[2]]) ^ ((uint32_t)MD2[y[2]] << 8) ^ ((uint32_t)y[2] << 16) ^ ((uint32_t)MD2[y[2]] << 24);
        cs->MDS4[i] = ((uint32_t)MD1[y[3]]) ^ ((uint32_t)y[3] << 8) ^ ((uint32_t)MD2[y[3]] << 16) ^ ((uint32_t)MD1[y[3]] << 24);
    }
}

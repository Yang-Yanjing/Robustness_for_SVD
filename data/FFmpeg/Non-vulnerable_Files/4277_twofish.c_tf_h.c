static uint32_t tf_h(uint32_t X, uint32_t L[4], int k)
{
    uint8_t y[4], l[4];
    AV_WL32(y, X);
    tf_h0(y, L, k);
    l[0] = y[0] ^ MD2[y[1]] ^ MD1[y[2]] ^ MD1[y[3]];
    l[1] = MD1[y[0]] ^ MD2[y[1]] ^ MD2[y[2]] ^ y[3];
    l[2] = MD2[y[0]] ^ MD1[y[1]] ^ y[2] ^ MD2[y[3]];
    l[3] = MD2[y[0]] ^ y[1] ^ MD2[y[2]] ^ MD1[y[3]];
    return AV_RL32(l);
}

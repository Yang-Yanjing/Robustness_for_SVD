static uint64_t F(uint64_t F_IN, uint64_t KE)
{
    KE ^= F_IN;
    F_IN=SP[0][KE >> 56]^SP[1][(KE >> 48) & MASK8]^SP[2][(KE >> 40) & MASK8]^SP[3][(KE >> 32) & MASK8]^SP[4][(KE >> 24) & MASK8]^SP[5][(KE >> 16) & MASK8]^SP[6][(KE >> 8) & MASK8]^SP[7][KE & MASK8];
    return F_IN;
}

static uint64_t des_encdec(uint64_t in, uint64_t K[16], int decrypt) {
    int i;
    
    decrypt = decrypt ? 15 : 0;
    
    in = shuffle(in, IP_shuffle, sizeof(IP_shuffle));
    for (i = 0; i < 16; i++) {
        uint32_t f_res;
        f_res = f_func(in, K[decrypt ^ i]);
        in = (in << 32) | (in >> 32);
        in ^= f_res;
    }
    in = (in << 32) | (in >> 32);
    
    in = shuffle_inv(in, IP_shuffle, sizeof(IP_shuffle));
    return in;
}

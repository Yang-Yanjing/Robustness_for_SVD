static inline void mix(av_aes_block state[2], uint32_t multbl[][256], int s1, int s3){
    uint8_t (*src)[4] = state[1].u8x4;
    state[0].u32[0] = mix_core(multbl, src[0][0], src[s1  ][1], src[2][2], src[s3  ][3]);
    state[0].u32[1] = mix_core(multbl, src[1][0], src[s3-1][1], src[3][2], src[s1-1][3]);
    state[0].u32[2] = mix_core(multbl, src[2][0], src[s3  ][1], src[0][2], src[s1  ][3]);
    state[0].u32[3] = mix_core(multbl, src[3][0], src[s1-1][1], src[1][2], src[s3-1][3]);
}

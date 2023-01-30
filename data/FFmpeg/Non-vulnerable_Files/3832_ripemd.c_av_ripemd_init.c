av_cold int av_ripemd_init(AVRIPEMD *ctx, int bits)
{
    ctx->digest_len = bits >> 5;
    switch (bits) {
    case 128: 
        ctx->state[0] = 0x67452301;
        ctx->state[1] = 0xEFCDAB89;
        ctx->state[2] = 0x98BADCFE;
        ctx->state[3] = 0x10325476;
        ctx->transform = ripemd128_transform;
        break;
    case 160: 
        ctx->state[0] = 0x67452301;
        ctx->state[1] = 0xEFCDAB89;
        ctx->state[2] = 0x98BADCFE;
        ctx->state[3] = 0x10325476;
        ctx->state[4] = 0xC3D2E1F0;
        ctx->transform = ripemd160_transform;
        break;
    case 256: 
        ctx->state[0] = 0x67452301;
        ctx->state[1] = 0xEFCDAB89;
        ctx->state[2] = 0x98BADCFE;
        ctx->state[3] = 0x10325476;
        ctx->state[4] = 0x76543210;
        ctx->state[5] = 0xFEDCBA98;
        ctx->state[6] = 0x89ABCDEF;
        ctx->state[7] = 0x01234567;
        ctx->transform = ripemd256_transform;
        break;
    case 320: 
        ctx->state[0] = 0x67452301;
        ctx->state[1] = 0xEFCDAB89;
        ctx->state[2] = 0x98BADCFE;
        ctx->state[3] = 0x10325476;
        ctx->state[4] = 0xC3D2E1F0;
        ctx->state[5] = 0x76543210;
        ctx->state[6] = 0xFEDCBA98;
        ctx->state[7] = 0x89ABCDEF;
        ctx->state[8] = 0x01234567;
        ctx->state[9] = 0x3C2D1E0F;
        ctx->transform = ripemd320_transform;
        break;
    default:
        return -1;
    }
    ctx->count = 0;
    return 0;
}

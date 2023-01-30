av_cold int av_sha512_init(AVSHA512 *ctx, int bits)
{
    ctx->digest_len = bits >> 6;
    switch (bits) {
    case 224: 
        ctx->state[0] = UINT64_C(0x8C3D37C819544DA2);
        ctx->state[1] = UINT64_C(0x73E1996689DCD4D6);
        ctx->state[2] = UINT64_C(0x1DFAB7AE32FF9C82);
        ctx->state[3] = UINT64_C(0x679DD514582F9FCF);
        ctx->state[4] = UINT64_C(0x0F6D2B697BD44DA8);
        ctx->state[5] = UINT64_C(0x77E36F7304C48942);
        ctx->state[6] = UINT64_C(0x3F9D85A86A1D36C8);
        ctx->state[7] = UINT64_C(0x1112E6AD91D692A1);
        break;
    case 256: 
        ctx->state[0] = UINT64_C(0x22312194FC2BF72C);
        ctx->state[1] = UINT64_C(0x9F555FA3C84C64C2);
        ctx->state[2] = UINT64_C(0x2393B86B6F53B151);
        ctx->state[3] = UINT64_C(0x963877195940EABD);
        ctx->state[4] = UINT64_C(0x96283EE2A88EFFE3);
        ctx->state[5] = UINT64_C(0xBE5E1E2553863992);
        ctx->state[6] = UINT64_C(0x2B0199FC2C85B8AA);
        ctx->state[7] = UINT64_C(0x0EB72DDC81C52CA2);
        break;
    case 384: 
        ctx->state[0] = UINT64_C(0xCBBB9D5DC1059ED8);
        ctx->state[1] = UINT64_C(0x629A292A367CD507);
        ctx->state[2] = UINT64_C(0x9159015A3070DD17);
        ctx->state[3] = UINT64_C(0x152FECD8F70E5939);
        ctx->state[4] = UINT64_C(0x67332667FFC00B31);
        ctx->state[5] = UINT64_C(0x8EB44A8768581511);
        ctx->state[6] = UINT64_C(0xDB0C2E0D64F98FA7);
        ctx->state[7] = UINT64_C(0x47B5481DBEFA4FA4);
        break;
    case 512: 
        ctx->state[0] = UINT64_C(0x6A09E667F3BCC908);
        ctx->state[1] = UINT64_C(0xBB67AE8584CAA73B);
        ctx->state[2] = UINT64_C(0x3C6EF372FE94F82B);
        ctx->state[3] = UINT64_C(0xA54FF53A5F1D36F1);
        ctx->state[4] = UINT64_C(0x510E527FADE682D1);
        ctx->state[5] = UINT64_C(0x9B05688C2B3E6C1F);
        ctx->state[6] = UINT64_C(0x1F83D9ABFB41BD6B);
        ctx->state[7] = UINT64_C(0x5BE0CD19137E2179);
        break;
    default:
        return -1;
    }
    ctx->count = 0;
    return 0;
}

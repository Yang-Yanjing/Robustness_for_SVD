const int av_tea_size = sizeof(AVTEA);
void av_tea_init(AVTEA *ctx, const uint8_t key[16], int rounds)
{
    int i;
    for (i = 0; i < 4; i++)
        ctx->key[i] = AV_RB32(key + (i << 2));
    ctx->rounds = rounds;
}

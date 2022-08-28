static uint8_t do_colorkey_pixel(ColorkeyContext *ctx, uint8_t r, uint8_t g, uint8_t b)
{
    int dr = (int)r - ctx->colorkey_rgba[0];
    int dg = (int)g - ctx->colorkey_rgba[1];
    int db = (int)b - ctx->colorkey_rgba[2];
    double diff = sqrt((dr * dr + dg * dg + db * db) / (255.0 * 255.0));
    if (ctx->blend > 0.0001) {
        return av_clipd((diff - ctx->similarity) / ctx->blend, 0.0, 1.0) * 255.0;
    } else {
        return (diff > ctx->similarity) ? 255 : 0;
    }
}

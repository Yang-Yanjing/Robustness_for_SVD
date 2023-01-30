static int init(AVFilterContext *ctx)
{
    XBRContext *xbr = ctx->priv;
    static const xbrfunc_t xbrfuncs[] = {xbr2x, xbr3x, xbr4x};
    uint32_t c;
    int bg, rg, g;
    for (bg = -255; bg < 256; bg++) {
        for (rg = -255; rg < 256; rg++) {
            const uint32_t u = (uint32_t)((-169*rg + 500*bg)/1000) + 128;
            const uint32_t v = (uint32_t)(( 500*rg -  81*bg)/1000) + 128;
            int startg = FFMAX3(-bg, -rg, 0);
            int endg = FFMIN3(255-bg, 255-rg, 255);
            uint32_t y = (uint32_t)(( 299*rg + 1000*startg + 114*bg)/1000);
            c = bg + (rg<<16) + 0x010101 * startg;
            for (g = startg; g <= endg; g++) {
                xbr->rgbtoyuv[c] = ((y++) << 16) + (u << 8) + v;
                c+= 0x010101;
            }
        }
    }
    xbr->func = xbrfuncs[xbr->n - 2];
    return 0;
}

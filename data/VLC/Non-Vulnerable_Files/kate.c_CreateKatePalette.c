}
static void CreateKatePalette( video_palette_t *fmt_palette, const kate_palette *palette )
{
    size_t n;
    fmt_palette->i_entries = palette->ncolors;
    for( n=0; n<palette->ncolors; ++n )
    {
        rgb_to_yuv(
            &fmt_palette->palette[n][0], &fmt_palette->palette[n][1], &fmt_palette->palette[n][2],
            palette->colors[n].r, palette->colors[n].g, palette->colors[n].b
        );
        fmt_palette->palette[n][3] = palette->colors[n].a;
    }
}

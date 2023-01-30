}
static void FilterS16N (audio_volume_t *vol, block_t *block, float volume)
{
    int16_t *p = (int16_t *)block->p_buffer;
    int_fast16_t mult = lroundf (volume * 0x1.p8f);
    if (mult == (1 << 8))
        return;
    for (size_t n = block->i_buffer / sizeof (*p); n > 0; n--)
    {
        int_fast32_t s = (*p * (int_fast32_t)mult) >> 8;
        if (s > INT16_MAX)
            s = INT16_MAX;
        else
        if (s < INT16_MIN)
            s = INT16_MIN;
        *(p++) = s;
    }
    (void) vol;
}

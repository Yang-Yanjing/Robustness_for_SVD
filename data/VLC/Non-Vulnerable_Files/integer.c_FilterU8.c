}
static void FilterU8 (audio_volume_t *vol, block_t *block, float volume)
{
    uint8_t *p = (uint8_t *)block->p_buffer;
    int_fast16_t mult = lroundf (volume * 0x1.p8f);
    if (mult == (1 << 8))
        return;
    for (size_t n = block->i_buffer / sizeof (*p); n > 0; n--)
    {
        int_fast32_t s = (((int_fast8_t)(*p - 128)) * (int_fast32_t)mult) >> 8;
        if (s > INT8_MAX)
            s = INT8_MAX;
        else
        if (s < INT8_MIN)
            s = INT8_MIN;
        *(p++) = s + 128;
    }
    (void) vol;
}

vlc_module_end ()
static void FilterS32N (audio_volume_t *vol, block_t *block, float volume)
{
    int32_t *p = (int32_t *)block->p_buffer;
    int_fast32_t mult = lroundf (volume * 0x1.p24f);
    if (mult == (1 << 24))
        return;
    for (size_t n = block->i_buffer / sizeof (*p); n > 0; n--)
    {
        int_fast64_t s = (*p * (int_fast64_t)mult) >> INT64_C(24);
        if (s > INT32_MAX)
            s = INT32_MAX;
        else
        if (s < INT32_MIN)
            s = INT32_MIN;
        *(p++) = s;
    }
    (void) vol;
}

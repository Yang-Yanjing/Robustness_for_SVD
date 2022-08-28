static void make_sin_table(int16_t *sin)
{
    unsigned half_pi = 1 << (LOG_PERIOD - 2);
    unsigned ampls = AMPLITUDE << AMPLITUDE_SHIFT;
    uint64_t unit2 = (uint64_t)(ampls * ampls) << 32;
    unsigned step, i, c, s, k, new_k, n2;
    

    sin[0] = 0;
    sin[half_pi] = ampls;
    for (step = half_pi; step > 1; step /= 2) {
        

        k = 0x10000;
        for (i = 0; i < half_pi / 2; i += step) {
            s = sin[i] + sin[i + step];
            c = sin[half_pi - i] + sin[half_pi - i - step];
            n2 = s * s + c * c;
            
            while (1) {
                new_k = (k + unit2 / ((uint64_t)k * n2) + 1) >> 1;
                if (k == new_k)
                    break;
                k = new_k;
            }
            sin[i + step / 2] = (k * s + 0x7FFF) >> 16;
            sin[half_pi - i - step / 2] = (k * c + 0x8000) >> 16;
        }
    }
    
    for (i = 0; i <= half_pi; i++)
        sin[i] = (sin[i] + (1 << (AMPLITUDE_SHIFT - 1))) >> AMPLITUDE_SHIFT;
    
    for (i = 0; i < half_pi; i++)
        sin[half_pi * 2 - i] = sin[i];
    for (i = 0; i < 2 * half_pi; i++)
        sin[i + 2 * half_pi] = -sin[i];
}

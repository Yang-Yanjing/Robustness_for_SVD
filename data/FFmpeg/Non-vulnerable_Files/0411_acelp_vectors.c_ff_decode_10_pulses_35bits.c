void ff_decode_10_pulses_35bits(const int16_t *fixed_index,
                                AMRFixed *fixed_sparse,
                                const uint8_t *gray_decode,
                                int half_pulse_count, int bits)
{
    int i;
    int mask = (1 << bits) - 1;
    fixed_sparse->no_repeat_mask = 0;
    fixed_sparse->n = 2 * half_pulse_count;
    for (i = 0; i < half_pulse_count; i++) {
        const int pos1   = gray_decode[fixed_index[2*i+1] & mask] + i;
        const int pos2   = gray_decode[fixed_index[2*i  ] & mask] + i;
        const float sign = (fixed_index[2*i+1] & (1 << bits)) ? -1.0 : 1.0;
        fixed_sparse->x[2*i+1] = pos1;
        fixed_sparse->x[2*i  ] = pos2;
        fixed_sparse->y[2*i+1] = sign;
        fixed_sparse->y[2*i  ] = pos2 < pos1 ? -sign : sign;
    }
}

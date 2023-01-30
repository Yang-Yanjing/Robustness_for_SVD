int ff_ac3_compute_bit_allocation(AC3EncodeContext *s)
{
    count_frame_bits(s);
    s->exponent_bits = count_exponent_bits(s);
    bit_alloc_masking(s);
    return cbr_bit_allocation(s);
}

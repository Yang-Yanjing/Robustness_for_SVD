static av_cold void dither_init(DitherDSPContext *ddsp,
                                enum AVResampleDitherMethod method)
{
    ddsp->quantize      = quantize_c;
    ddsp->ptr_align     = 1;
    ddsp->samples_align = 1;
    if (method == AV_RESAMPLE_DITHER_RECTANGULAR)
        ddsp->dither_int_to_float = dither_int_to_float_rectangular_c;
    else
        ddsp->dither_int_to_float = dither_int_to_float_triangular_c;
    if (ARCH_X86)
        ff_dither_init_x86(ddsp, method);
}

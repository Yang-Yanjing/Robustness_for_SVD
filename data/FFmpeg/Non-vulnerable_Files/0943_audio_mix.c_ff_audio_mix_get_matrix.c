int ff_audio_mix_get_matrix(AudioMix *am, double *matrix, int stride)
{
    int i, o, i0, o0;
    if ( am->in_channels <= 0 ||  am->in_channels > AVRESAMPLE_MAX_CHANNELS ||
        am->out_channels <= 0 || am->out_channels > AVRESAMPLE_MAX_CHANNELS) {
        av_log(am->avr, AV_LOG_ERROR, "Invalid channel counts\n");
        return AVERROR(EINVAL);
    }
#define GET_MATRIX_CONVERT(suffix, scale)                                   \
    if (!am->matrix_ ## suffix[0]) {                                        \
        av_log(am->avr, AV_LOG_ERROR, "matrix is not set\n");               \
        return AVERROR(EINVAL);                                             \
    }                                                                       \
    for (o = 0, o0 = 0; o < am->out_channels; o++) {                        \
        for (i = 0, i0 = 0; i < am->in_channels; i++) {                     \
            if (am->input_skip[i] || am->output_zero[o])                    \
                matrix[o * stride + i] = 0.0;                               \
            else                                                            \
                matrix[o * stride + i] = am->matrix_ ## suffix[o0][i0] *    \
                                         (scale);                           \
            if (!am->input_skip[i])                                         \
                i0++;                                                       \
        }                                                                   \
        if (!am->output_zero[o])                                            \
            o0++;                                                           \
    }
    switch (am->coeff_type) {
    case AV_MIX_COEFF_TYPE_Q8:
        GET_MATRIX_CONVERT(q8, 1.0 / 256.0);
        break;
    case AV_MIX_COEFF_TYPE_Q15:
        GET_MATRIX_CONVERT(q15, 1.0 / 32768.0);
        break;
    case AV_MIX_COEFF_TYPE_FLT:
        GET_MATRIX_CONVERT(flt, 1.0);
        break;
    default:
        av_log(am->avr, AV_LOG_ERROR, "Invalid mix coeff type\n");
        return AVERROR(EINVAL);
    }
    return 0;
}

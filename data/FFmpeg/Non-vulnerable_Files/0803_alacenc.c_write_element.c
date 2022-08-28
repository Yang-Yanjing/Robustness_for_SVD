static void write_element(AlacEncodeContext *s,
                          enum AlacRawDataBlockType element, int instance,
                          const uint8_t *samples0, const uint8_t *samples1)
{
    uint8_t const *samples[2] = { samples0, samples1 };
    int i, j, channels;
    int prediction_type = 0;
    PutBitContext *pb = &s->pbctx;
    channels = element == TYPE_CPE ? 2 : 1;
    if (s->verbatim) {
        write_element_header(s, element, instance);
        
        if (s->avctx->sample_fmt == AV_SAMPLE_FMT_S32P) {
            int shift = 32 - s->avctx->bits_per_raw_sample;
            int32_t const *samples_s32[2] = { (const int32_t *)samples0,
                                              (const int32_t *)samples1 };
            for (i = 0; i < s->frame_size; i++)
                for (j = 0; j < channels; j++)
                    put_sbits(pb, s->avctx->bits_per_raw_sample,
                              samples_s32[j][i] >> shift);
        } else {
            int16_t const *samples_s16[2] = { (const int16_t *)samples0,
                                              (const int16_t *)samples1 };
            for (i = 0; i < s->frame_size; i++)
                for (j = 0; j < channels; j++)
                    put_sbits(pb, s->avctx->bits_per_raw_sample,
                              samples_s16[j][i]);
        }
    } else {
        s->write_sample_size = s->avctx->bits_per_raw_sample - s->extra_bits +
                               channels - 1;
        init_sample_buffers(s, channels, samples);
        write_element_header(s, element, instance);
        
        if (s->extra_bits) {
            uint32_t mask = (1 << s->extra_bits) - 1;
            for (j = 0; j < channels; j++) {
                int32_t *extra = s->predictor_buf[j];
                int32_t *smp   = s->sample_buf[j];
                for (i = 0; i < s->frame_size; i++) {
                    extra[i] = smp[i] & mask;
                    smp[i] >>= s->extra_bits;
                }
            }
        }
        if (channels == 2)
            alac_stereo_decorrelation(s);
        else
            s->interlacing_shift = s->interlacing_leftweight = 0;
        put_bits(pb, 8, s->interlacing_shift);
        put_bits(pb, 8, s->interlacing_leftweight);
        for (i = 0; i < channels; i++) {
            calc_predictor_params(s, i);
            put_bits(pb, 4, prediction_type);
            put_bits(pb, 4, s->lpc[i].lpc_quant);
            put_bits(pb, 3, s->rc.rice_modifier);
            put_bits(pb, 5, s->lpc[i].lpc_order);
            
            for (j = 0; j < s->lpc[i].lpc_order; j++)
                put_sbits(pb, 16, s->lpc[i].lpc_coeff[j]);
        }
        
        if (s->extra_bits) {
            for (i = 0; i < s->frame_size; i++) {
                for (j = 0; j < channels; j++) {
                    put_bits(pb, s->extra_bits, s->predictor_buf[j][i]);
                }
            }
        }
        
        for (i = 0; i < channels; i++) {
            alac_linear_predictor(s, i);
            
            if (prediction_type == 15) {
                
                int32_t *residual = s->predictor_buf[i];
                for (j = s->frame_size - 1; j > 0; j--)
                    residual[j] -= residual[j - 1];
            }
            alac_entropy_coder(s, i);
        }
    }
}

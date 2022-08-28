static int xa_decode(AVCodecContext *avctx, int16_t *out0, int16_t *out1,
                     const uint8_t *in, ADPCMChannelStatus *left,
                     ADPCMChannelStatus *right, int channels, int sample_offset)
{
    int i, j;
    int shift,filter,f0,f1;
    int s_1,s_2;
    int d,s,t;
    out0 += sample_offset;
    if (channels == 1)
        out1 = out0 + 28;
    else
        out1 += sample_offset;
    for(i=0;i<4;i++) {
        shift  = 12 - (in[4+i*2] & 15);
        filter = in[4+i*2] >> 4;
        if (filter >= FF_ARRAY_ELEMS(xa_adpcm_table)) {
            avpriv_request_sample(avctx, "unknown XA-ADPCM filter %d", filter);
            filter=0;
        }
        f0 = xa_adpcm_table[filter][0];
        f1 = xa_adpcm_table[filter][1];
        s_1 = left->sample1;
        s_2 = left->sample2;
        for(j=0;j<28;j++) {
            d = in[16+i+j*4];
            t = sign_extend(d, 4);
            s = ( t<<shift ) + ((s_1*f0 + s_2*f1+32)>>6);
            s_2 = s_1;
            s_1 = av_clip_int16(s);
            out0[j] = s_1;
        }
        if (channels == 2) {
            left->sample1 = s_1;
            left->sample2 = s_2;
            s_1 = right->sample1;
            s_2 = right->sample2;
        }
        shift  = 12 - (in[5+i*2] & 15);
        filter = in[5+i*2] >> 4;
        if (filter >= FF_ARRAY_ELEMS(xa_adpcm_table)) {
            avpriv_request_sample(avctx, "unknown XA-ADPCM filter %d", filter);
            filter=0;
        }
        f0 = xa_adpcm_table[filter][0];
        f1 = xa_adpcm_table[filter][1];
        for(j=0;j<28;j++) {
            d = in[16+i+j*4];
            t = sign_extend(d >> 4, 4);
            s = ( t<<shift ) + ((s_1*f0 + s_2*f1+32)>>6);
            s_2 = s_1;
            s_1 = av_clip_int16(s);
            out1[j] = s_1;
        }
        if (channels == 2) {
            right->sample1 = s_1;
            right->sample2 = s_2;
        } else {
            left->sample1 = s_1;
            left->sample2 = s_2;
        }
        out0 += 28 * (3 - channels);
        out1 += 28 * (3 - channels);
    }
    return 0;
}

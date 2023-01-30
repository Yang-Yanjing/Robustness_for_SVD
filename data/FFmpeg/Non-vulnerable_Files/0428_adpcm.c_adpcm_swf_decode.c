static void adpcm_swf_decode(AVCodecContext *avctx, const uint8_t *buf, int buf_size, int16_t *samples)
{
    ADPCMDecodeContext *c = avctx->priv_data;
    GetBitContext gb;
    const int *table;
    int k0, signmask, nb_bits, count;
    int size = buf_size*8;
    int i;
    init_get_bits(&gb, buf, size);
    
    nb_bits = get_bits(&gb, 2)+2;
    table = swf_index_tables[nb_bits-2];
    k0 = 1 << (nb_bits-2);
    signmask = 1 << (nb_bits-1);
    while (get_bits_count(&gb) <= size - 22*avctx->channels) {
        for (i = 0; i < avctx->channels; i++) {
            *samples++ = c->status[i].predictor = get_sbits(&gb, 16);
            c->status[i].step_index = get_bits(&gb, 6);
        }
        for (count = 0; get_bits_count(&gb) <= size - nb_bits*avctx->channels && count < 4095; count++) {
            int i;
            for (i = 0; i < avctx->channels; i++) {
                
                int delta = get_bits(&gb, nb_bits);
                int step = ff_adpcm_step_table[c->status[i].step_index];
                long vpdiff = 0; 
                int k = k0;
                do {
                    if (delta & k)
                        vpdiff += step;
                    step >>= 1;
                    k >>= 1;
                } while(k);
                vpdiff += step;
                if (delta & signmask)
                    c->status[i].predictor -= vpdiff;
                else
                    c->status[i].predictor += vpdiff;
                c->status[i].step_index += table[delta & (~signmask)];
                c->status[i].step_index = av_clip(c->status[i].step_index, 0, 88);
                c->status[i].predictor = av_clip_int16(c->status[i].predictor);
                *samples++ = c->status[i].predictor;
            }
        }
    }
}

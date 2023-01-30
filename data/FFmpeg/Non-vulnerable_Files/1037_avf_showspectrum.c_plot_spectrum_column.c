static int plot_spectrum_column(AVFilterLink *inlink, AVFrame *insamples)
{
    int ret;
    AVFilterContext *ctx = inlink->dst;
    AVFilterLink *outlink = ctx->outputs[0];
    ShowSpectrumContext *s = ctx->priv;
    AVFrame *outpicref = s->outpicref;
    

    const int nb_freq = 1 << (s->rdft_bits - 1);
    const int win_size = nb_freq << 1;
    const double w = 1. / (sqrt(nb_freq) * 32768.);
    int h = s->channel_height;
    int ch, plane, n, y;
    av_assert0(insamples->nb_samples == win_size);
    
    for (ch = 0; ch < s->nb_display_channels; ch++) {
        const int16_t *p = (int16_t *)insamples->extended_data[ch];
        for (n = 0; n < win_size; n++)
            s->rdft_data[ch][n] = p[n] * s->window_func_lut[n];
    }
    
    for (ch = 0; ch < s->nb_display_channels; ch++)
        av_rdft_calc(s->rdft, s->rdft_data[ch]);
    
#define RE(y, ch) s->rdft_data[ch][2 * (y) + 0]
#define IM(y, ch) s->rdft_data[ch][2 * (y) + 1]
#define MAGNITUDE(y, ch) hypot(RE(y, ch), IM(y, ch))
    
    for (y = 0; y < outlink->h; y++) {
        s->combine_buffer[3 * y    ] = 0;
        s->combine_buffer[3 * y + 1] = 127.5;
        s->combine_buffer[3 * y + 2] = 127.5;
    }
    for (ch = 0; ch < s->nb_display_channels; ch++) {
        float yf, uf, vf;
        
        switch (s->mode) {
        case COMBINED:
            
            yf = 256.0f / s->nb_display_channels;
            switch (s->color_mode) {
            case INTENSITY:
                uf = yf;
                vf = yf;
                break;
            case CHANNEL:
                
                
                uf = yf * M_PI;
                vf = yf * M_PI;
                break;
            default:
                av_assert0(0);
            }
            break;
        case SEPARATE:
            
            yf = 256.0f;
            uf = 256.0f;
            vf = 256.0f;
            break;
        default:
            av_assert0(0);
        }
        if (s->color_mode == CHANNEL) {
            if (s->nb_display_channels > 1) {
                uf *= 0.5 * sin((2 * M_PI * ch) / s->nb_display_channels);
                vf *= 0.5 * cos((2 * M_PI * ch) / s->nb_display_channels);
            } else {
                uf = 0.0f;
                vf = 0.0f;
            }
        }
        uf *= s->saturation;
        vf *= s->saturation;
        
        for (y = 0; y < h; y++) {
            int row = (s->mode == COMBINED) ? y : ch * h + y;
            float *out = &s->combine_buffer[3 * row];
            
            float a = w * MAGNITUDE(y, ch);
            
            switch (s->scale) {
            case LINEAR:
                break;
            case SQRT:
                a = sqrt(a);
                break;
            case CBRT:
                a = cbrt(a);
                break;
            case LOG:
                a = 1 - log(FFMAX(FFMIN(1, a), 1e-6)) / log(1e-6); 
                break;
            default:
                av_assert0(0);
            }
            if (s->color_mode == INTENSITY) {
                float y, u, v;
                int i;
                for (i = 1; i < sizeof(intensity_color_table) / sizeof(*intensity_color_table) - 1; i++)
                    if (intensity_color_table[i].a >= a)
                        break;
                
                
                if (a <= intensity_color_table[i - 1].a) {
                    y = intensity_color_table[i - 1].y;
                    u = intensity_color_table[i - 1].u;
                    v = intensity_color_table[i - 1].v;
                } else if (a >= intensity_color_table[i].a) {
                    y = intensity_color_table[i].y;
                    u = intensity_color_table[i].u;
                    v = intensity_color_table[i].v;
                } else {
                    float start = intensity_color_table[i - 1].a;
                    float end = intensity_color_table[i].a;
                    float lerpfrac = (a - start) / (end - start);
                    y = intensity_color_table[i - 1].y * (1.0f - lerpfrac)
                      + intensity_color_table[i].y * lerpfrac;
                    u = intensity_color_table[i - 1].u * (1.0f - lerpfrac)
                      + intensity_color_table[i].u * lerpfrac;
                    v = intensity_color_table[i - 1].v * (1.0f - lerpfrac)
                      + intensity_color_table[i].v * lerpfrac;
                }
                out[0] += y * yf;
                out[1] += u * uf;
                out[2] += v * vf;
            } else {
                out[0] += a * yf;
                out[1] += a * uf;
                out[2] += a * vf;
            }
        }
    }
    
    if (s->sliding == SCROLL) {
        for (plane = 0; plane < 3; plane++) {
            for (y = 0; y < outlink->h; y++) {
                uint8_t *p = outpicref->data[plane] +
                             y * outpicref->linesize[plane];
                memmove(p, p + 1, outlink->w - 1);
            }
        }
        s->xpos = outlink->w - 1;
    }
    for (plane = 0; plane < 3; plane++) {
        uint8_t *p = outpicref->data[plane] +
                     (outlink->h - 1) * outpicref->linesize[plane] +
                     s->xpos;
        for (y = 0; y < outlink->h; y++) {
            *p = rint(FFMAX(0, FFMIN(s->combine_buffer[3 * y + plane], 255)));
            p -= outpicref->linesize[plane];
        }
    }
    if (s->sliding != FULLFRAME || s->xpos == 0)
        outpicref->pts = insamples->pts;
    s->xpos++;
    if (s->xpos >= outlink->w)
        s->xpos = 0;
    if (s->sliding != FULLFRAME || s->xpos == 0) {
        s->req_fullfilled = 1;
        ret = ff_filter_frame(outlink, av_frame_clone(s->outpicref));
        if (ret < 0)
            return ret;
    }
    return win_size;
}

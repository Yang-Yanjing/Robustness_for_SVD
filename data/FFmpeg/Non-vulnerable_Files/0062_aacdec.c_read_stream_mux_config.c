static int read_stream_mux_config(struct LATMContext *latmctx,
                                  GetBitContext *gb)
{
    int ret, audio_mux_version = get_bits(gb, 1);
    latmctx->audio_mux_version_A = 0;
    if (audio_mux_version)
        latmctx->audio_mux_version_A = get_bits(gb, 1);
    if (!latmctx->audio_mux_version_A) {
        if (audio_mux_version)
            latm_get_value(gb);                 
        skip_bits(gb, 1);                       
        skip_bits(gb, 6);                       
        
        if (get_bits(gb, 4)) {                  
            avpriv_request_sample(latmctx->aac_ctx.avctx, "Multiple programs");
            return AVERROR_PATCHWELCOME;
        }
        
        
        if (get_bits(gb, 3)) {                   
            avpriv_request_sample(latmctx->aac_ctx.avctx, "Multiple layers");
            return AVERROR_PATCHWELCOME;
        }
        
        if (!audio_mux_version) {
            if ((ret = latm_decode_audio_specific_config(latmctx, gb, 0)) < 0)
                return ret;
        } else {
            int ascLen = latm_get_value(gb);
            if ((ret = latm_decode_audio_specific_config(latmctx, gb, ascLen)) < 0)
                return ret;
            ascLen -= ret;
            skip_bits_long(gb, ascLen);
        }
        latmctx->frame_length_type = get_bits(gb, 3);
        switch (latmctx->frame_length_type) {
        case 0:
            skip_bits(gb, 8);       
            break;
        case 1:
            latmctx->frame_length = get_bits(gb, 9);
            break;
        case 3:
        case 4:
        case 5:
            skip_bits(gb, 6);       
            break;
        case 6:
        case 7:
            skip_bits(gb, 1);       
            break;
        }
        if (get_bits(gb, 1)) {                  
            if (audio_mux_version) {
                latm_get_value(gb);             
            } else {
                int esc;
                do {
                    esc = get_bits(gb, 1);
                    skip_bits(gb, 8);
                } while (esc);
            }
        }
        if (get_bits(gb, 1))                     
            skip_bits(gb, 8);                    
    }
    return 0;
}

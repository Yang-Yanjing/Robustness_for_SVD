static int decode_vol_header(Mpeg4DecContext *ctx, GetBitContext *gb)
{
    MpegEncContext *s = &ctx->m;
    int width, height, vo_ver_id;
    
    skip_bits(gb, 1);                   
    s->vo_type = get_bits(gb, 8);
    if (get_bits1(gb) != 0) {           
        vo_ver_id = get_bits(gb, 4);    
        skip_bits(gb, 3);               
    } else {
        vo_ver_id = 1;
    }
    s->aspect_ratio_info = get_bits(gb, 4);
    if (s->aspect_ratio_info == FF_ASPECT_EXTENDED) {
        s->avctx->sample_aspect_ratio.num = get_bits(gb, 8);  
        s->avctx->sample_aspect_ratio.den = get_bits(gb, 8);  
    } else {
        s->avctx->sample_aspect_ratio = ff_h263_pixel_aspect[s->aspect_ratio_info];
    }
    if ((ctx->vol_control_parameters = get_bits1(gb))) { 
        int chroma_format = get_bits(gb, 2);
        if (chroma_format != CHROMA_420)
            av_log(s->avctx, AV_LOG_ERROR, "illegal chroma format\n");
        s->low_delay = get_bits1(gb);
        if (get_bits1(gb)) {    
            get_bits(gb, 15);   
            check_marker(gb, "after first_half_bitrate");
            get_bits(gb, 15);   
            check_marker(gb, "after latter_half_bitrate");
            get_bits(gb, 15);   
            check_marker(gb, "after first_half_vbv_buffer_size");
            get_bits(gb, 3);    
            get_bits(gb, 11);   
            check_marker(gb, "after first_half_vbv_occupancy");
            get_bits(gb, 15);   
            check_marker(gb, "after latter_half_vbv_occupancy");
        }
    } else {
        

        if (s->picture_number == 0)
            s->low_delay = 0;
    }
    ctx->shape = get_bits(gb, 2); 
    if (ctx->shape != RECT_SHAPE)
        av_log(s->avctx, AV_LOG_ERROR, "only rectangular vol supported\n");
    if (ctx->shape == GRAY_SHAPE && vo_ver_id != 1) {
        av_log(s->avctx, AV_LOG_ERROR, "Gray shape not supported\n");
        skip_bits(gb, 4);  
    }
    check_marker(gb, "before time_increment_resolution");
    s->avctx->framerate.num = get_bits(gb, 16);
    if (!s->avctx->framerate.num) {
        av_log(s->avctx, AV_LOG_ERROR, "framerate==0\n");
        return AVERROR_INVALIDDATA;
    }
    ctx->time_increment_bits = av_log2(s->avctx->framerate.num - 1) + 1;
    if (ctx->time_increment_bits < 1)
        ctx->time_increment_bits = 1;
    check_marker(gb, "before fixed_vop_rate");
    if (get_bits1(gb) != 0)     
        s->avctx->framerate.den = get_bits(gb, ctx->time_increment_bits);
    else
        s->avctx->framerate.den = 1;
    s->avctx->time_base = av_inv_q(av_mul_q(s->avctx->framerate, (AVRational){s->avctx->ticks_per_frame, 1}));
    ctx->t_frame = 0;
    if (ctx->shape != BIN_ONLY_SHAPE) {
        if (ctx->shape == RECT_SHAPE) {
            check_marker(gb, "before width");
            width = get_bits(gb, 13);
            check_marker(gb, "before height");
            height = get_bits(gb, 13);
            check_marker(gb, "after height");
            if (width && height &&  
                !(s->width && s->codec_tag == AV_RL32("MP4S"))) {
                if (s->width && s->height &&
                    (s->width != width || s->height != height))
                    s->context_reinit = 1;
                s->width  = width;
                s->height = height;
            }
        }
        s->progressive_sequence  =
        s->progressive_frame     = get_bits1(gb) ^ 1;
        s->interlaced_dct        = 0;
        if (!get_bits1(gb) && (s->avctx->debug & FF_DEBUG_PICT_INFO))
            av_log(s->avctx, AV_LOG_INFO,           
                   "MPEG4 OBMC not supported (very likely buggy encoder)\n");
        if (vo_ver_id == 1)
            ctx->vol_sprite_usage = get_bits1(gb);    
        else
            ctx->vol_sprite_usage = get_bits(gb, 2);  
        if (ctx->vol_sprite_usage == STATIC_SPRITE)
            av_log(s->avctx, AV_LOG_ERROR, "Static Sprites not supported\n");
        if (ctx->vol_sprite_usage == STATIC_SPRITE ||
            ctx->vol_sprite_usage == GMC_SPRITE) {
            if (ctx->vol_sprite_usage == STATIC_SPRITE) {
                skip_bits(gb, 13); 
                check_marker(gb, "after sprite_width");
                skip_bits(gb, 13); 
                check_marker(gb, "after sprite_height");
                skip_bits(gb, 13); 
                check_marker(gb, "after sprite_left");
                skip_bits(gb, 13); 
                check_marker(gb, "after sprite_top");
            }
            ctx->num_sprite_warping_points = get_bits(gb, 6);
            if (ctx->num_sprite_warping_points > 3) {
                av_log(s->avctx, AV_LOG_ERROR,
                       "%d sprite_warping_points\n",
                       ctx->num_sprite_warping_points);
                ctx->num_sprite_warping_points = 0;
                return AVERROR_INVALIDDATA;
            }
            s->sprite_warping_accuracy  = get_bits(gb, 2);
            ctx->sprite_brightness_change = get_bits1(gb);
            if (ctx->vol_sprite_usage == STATIC_SPRITE)
                skip_bits1(gb); 
        }
        
        if (get_bits1(gb) == 1) {                   
            s->quant_precision = get_bits(gb, 4);   
            if (get_bits(gb, 4) != 8)               
                av_log(s->avctx, AV_LOG_ERROR, "N-bit not supported\n");
            if (s->quant_precision != 5)
                av_log(s->avctx, AV_LOG_ERROR,
                       "quant precision %d\n", s->quant_precision);
            if (s->quant_precision<3 || s->quant_precision>9) {
                s->quant_precision = 5;
            }
        } else {
            s->quant_precision = 5;
        }
        
        if ((s->mpeg_quant = get_bits1(gb))) { 
            int i, v;
            
            for (i = 0; i < 64; i++) {
                int j = s->idsp.idct_permutation[i];
                v = ff_mpeg4_default_intra_matrix[i];
                s->intra_matrix[j]        = v;
                s->chroma_intra_matrix[j] = v;
                v = ff_mpeg4_default_non_intra_matrix[i];
                s->inter_matrix[j]        = v;
                s->chroma_inter_matrix[j] = v;
            }
            
            if (get_bits1(gb)) {
                int last = 0;
                for (i = 0; i < 64; i++) {
                    int j;
                    v = get_bits(gb, 8);
                    if (v == 0)
                        break;
                    last = v;
                    j = s->idsp.idct_permutation[ff_zigzag_direct[i]];
                    s->intra_matrix[j]        = last;
                    s->chroma_intra_matrix[j] = last;
                }
                
                for (; i < 64; i++) {
                    int j = s->idsp.idct_permutation[ff_zigzag_direct[i]];
                    s->intra_matrix[j]        = last;
                    s->chroma_intra_matrix[j] = last;
                }
            }
            
            if (get_bits1(gb)) {
                int last = 0;
                for (i = 0; i < 64; i++) {
                    int j;
                    v = get_bits(gb, 8);
                    if (v == 0)
                        break;
                    last = v;
                    j = s->idsp.idct_permutation[ff_zigzag_direct[i]];
                    s->inter_matrix[j]        = v;
                    s->chroma_inter_matrix[j] = v;
                }
                
                for (; i < 64; i++) {
                    int j = s->idsp.idct_permutation[ff_zigzag_direct[i]];
                    s->inter_matrix[j]        = last;
                    s->chroma_inter_matrix[j] = last;
                }
            }
            
        }
        if (vo_ver_id != 1)
            s->quarter_sample = get_bits1(gb);
        else
            s->quarter_sample = 0;
        if (get_bits_left(gb) < 4) {
            av_log(s->avctx, AV_LOG_ERROR, "VOL Header truncated\n");
            return AVERROR_INVALIDDATA;
        }
        if (!get_bits1(gb)) {
            int pos               = get_bits_count(gb);
            int estimation_method = get_bits(gb, 2);
            if (estimation_method < 2) {
                if (!get_bits1(gb)) {
                    ctx->cplx_estimation_trash_i += 8 * get_bits1(gb);  
                    ctx->cplx_estimation_trash_i += 8 * get_bits1(gb);  
                    ctx->cplx_estimation_trash_i += 8 * get_bits1(gb);  
                    ctx->cplx_estimation_trash_i += 8 * get_bits1(gb);  
                    ctx->cplx_estimation_trash_i += 8 * get_bits1(gb);  
                    ctx->cplx_estimation_trash_i += 8 * get_bits1(gb);  
                }
                if (!get_bits1(gb)) {
                    ctx->cplx_estimation_trash_i += 8 * get_bits1(gb);  
                    ctx->cplx_estimation_trash_p += 8 * get_bits1(gb);  
                    ctx->cplx_estimation_trash_p += 8 * get_bits1(gb);  
                    ctx->cplx_estimation_trash_i += 8 * get_bits1(gb);  
                }
                if (!check_marker(gb, "in complexity estimation part 1")) {
                    skip_bits_long(gb, pos - get_bits_count(gb));
                    goto no_cplx_est;
                }
                if (!get_bits1(gb)) {
                    ctx->cplx_estimation_trash_i += 8 * get_bits1(gb);  
                    ctx->cplx_estimation_trash_i += 8 * get_bits1(gb);  
                    ctx->cplx_estimation_trash_i += 8 * get_bits1(gb);  
                    ctx->cplx_estimation_trash_i += 4 * get_bits1(gb);  
                }
                if (!get_bits1(gb)) {
                    ctx->cplx_estimation_trash_p += 8 * get_bits1(gb);  
                    ctx->cplx_estimation_trash_p += 8 * get_bits1(gb);  
                    ctx->cplx_estimation_trash_b += 8 * get_bits1(gb);  
                    ctx->cplx_estimation_trash_p += 8 * get_bits1(gb);  
                    ctx->cplx_estimation_trash_p += 8 * get_bits1(gb);  
                    ctx->cplx_estimation_trash_p += 8 * get_bits1(gb);  
                }
                if (!check_marker(gb, "in complexity estimation part 2")) {
                    skip_bits_long(gb, pos - get_bits_count(gb));
                    goto no_cplx_est;
                }
                if (estimation_method == 1) {
                    ctx->cplx_estimation_trash_i += 8 * get_bits1(gb);  
                    ctx->cplx_estimation_trash_p += 8 * get_bits1(gb);  
                }
            } else
                av_log(s->avctx, AV_LOG_ERROR,
                       "Invalid Complexity estimation method %d\n",
                       estimation_method);
        } else {
no_cplx_est:
            ctx->cplx_estimation_trash_i =
            ctx->cplx_estimation_trash_p =
            ctx->cplx_estimation_trash_b = 0;
        }
        ctx->resync_marker = !get_bits1(gb); 
        s->data_partitioning = get_bits1(gb);
        if (s->data_partitioning)
            ctx->rvlc = get_bits1(gb);
        if (vo_ver_id != 1) {
            ctx->new_pred = get_bits1(gb);
            if (ctx->new_pred) {
                av_log(s->avctx, AV_LOG_ERROR, "new pred not supported\n");
                skip_bits(gb, 2); 
                skip_bits1(gb);   
            }
            if (get_bits1(gb)) 
                av_log(s->avctx, AV_LOG_ERROR,
                       "reduced resolution VOP not supported\n");
        } else {
            ctx->new_pred = 0;
        }
        ctx->scalability = get_bits1(gb);
        if (ctx->scalability) {
            GetBitContext bak = *gb;
            int h_sampling_factor_n;
            int h_sampling_factor_m;
            int v_sampling_factor_n;
            int v_sampling_factor_m;
            skip_bits1(gb);    
            skip_bits(gb, 4);  
            skip_bits1(gb);    
            h_sampling_factor_n = get_bits(gb, 5);
            h_sampling_factor_m = get_bits(gb, 5);
            v_sampling_factor_n = get_bits(gb, 5);
            v_sampling_factor_m = get_bits(gb, 5);
            ctx->enhancement_type = get_bits1(gb);
            if (h_sampling_factor_n == 0 || h_sampling_factor_m == 0 ||
                v_sampling_factor_n == 0 || v_sampling_factor_m == 0) {
                

                ctx->scalability = 0;
                *gb            = bak;
            } else
                av_log(s->avctx, AV_LOG_ERROR, "scalability not supported\n");
            
        }
    }
    if (s->avctx->debug&FF_DEBUG_PICT_INFO) {
        av_log(s->avctx, AV_LOG_DEBUG, "tb %d/%d, tincrbits:%d, qp_prec:%d, ps:%d,  %s%s%s%s\n",
               s->avctx->framerate.den, s->avctx->framerate.num,
               ctx->time_increment_bits,
               s->quant_precision,
               s->progressive_sequence,
               ctx->scalability ? "scalability " :"" , s->quarter_sample ? "qpel " : "",
               s->data_partitioning ? "partition " : "", ctx->rvlc ? "rvlc " : ""
        );
    }
    return 0;
}

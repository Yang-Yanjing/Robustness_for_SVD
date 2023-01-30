static int estimate_motion_thread(AVCodecContext *c, void *arg){
    MpegEncContext *s= *(void**)arg;
    ff_check_alignment();
    s->me.dia_size= s->avctx->dia_size;
    s->first_slice_line=1;
    for(s->mb_y= s->start_mb_y; s->mb_y < s->end_mb_y; s->mb_y++) {
        s->mb_x=0; 
        ff_init_block_index(s);
        for(s->mb_x=0; s->mb_x < s->mb_width; s->mb_x++) {
            s->block_index[0]+=2;
            s->block_index[1]+=2;
            s->block_index[2]+=2;
            s->block_index[3]+=2;
            
            if(s->pict_type==AV_PICTURE_TYPE_B)
                ff_estimate_b_frame_motion(s, s->mb_x, s->mb_y);
            else
                ff_estimate_p_frame_motion(s, s->mb_x, s->mb_y);
        }
        s->first_slice_line=0;
    }
    return 0;
}

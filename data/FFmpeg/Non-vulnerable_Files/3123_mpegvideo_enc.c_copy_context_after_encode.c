static inline void copy_context_after_encode(MpegEncContext *d, MpegEncContext *s, int type){
    int i;
    memcpy(d->mv, s->mv, 2*4*2*sizeof(int));
    memcpy(d->last_mv, s->last_mv, 2*2*2*sizeof(int)); 
    
    d->mb_skip_run= s->mb_skip_run;
    for(i=0; i<3; i++)
        d->last_dc[i] = s->last_dc[i];
    
    d->mv_bits= s->mv_bits;
    d->i_tex_bits= s->i_tex_bits;
    d->p_tex_bits= s->p_tex_bits;
    d->i_count= s->i_count;
    d->f_count= s->f_count;
    d->b_count= s->b_count;
    d->skip_count= s->skip_count;
    d->misc_bits= s->misc_bits;
    d->mb_intra= s->mb_intra;
    d->mb_skipped= s->mb_skipped;
    d->mv_type= s->mv_type;
    d->mv_dir= s->mv_dir;
    d->pb= s->pb;
    if(s->data_partitioning){
        d->pb2= s->pb2;
        d->tex_pb= s->tex_pb;
    }
    d->block= s->block;
    for(i=0; i<8; i++)
        d->block_last_index[i]= s->block_last_index[i];
    d->interlaced_dct= s->interlaced_dct;
    d->qscale= s->qscale;
    d->esc3_level_length= s->esc3_level_length;
}

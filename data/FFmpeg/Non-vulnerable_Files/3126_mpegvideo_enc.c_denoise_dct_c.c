static void denoise_dct_c(MpegEncContext *s, int16_t *block){
    const int intra= s->mb_intra;
    int i;
    s->dct_count[intra]++;
    for(i=0; i<64; i++){
        int level= block[i];
        if(level){
            if(level>0){
                s->dct_error_sum[intra][i] += level;
                level -= s->dct_offset[intra][i];
                if(level<0) level=0;
            }else{
                s->dct_error_sum[intra][i] -= level;
                level += s->dct_offset[intra][i];
                if(level>0) level=0;
            }
            block[i]= level;
        }
    }
}

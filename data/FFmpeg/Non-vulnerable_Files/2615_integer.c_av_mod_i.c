AVInteger av_mod_i(AVInteger *quot, AVInteger a, AVInteger b){
    int i= av_log2_i(a) - av_log2_i(b);
    AVInteger quot_temp;
    if(!quot) quot = &quot_temp;
    av_assert2((int16_t)a.v[AV_INTEGER_SIZE-1] >= 0 && (int16_t)b.v[AV_INTEGER_SIZE-1] >= 0);
    av_assert2(av_log2_i(b)>=0);
    if(i > 0)
        b= av_shr_i(b, -i);
    memset(quot, 0, sizeof(AVInteger));
    while(i-- >= 0){
        *quot= av_shr_i(*quot, -1);
        if(av_cmp_i(a, b) >= 0){
            a= av_sub_i(a, b);
            quot->v[0] += 1;
        }
        b= av_shr_i(b, 1);
    }
    return a;
}

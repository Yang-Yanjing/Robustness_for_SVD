int av_log2_i(AVInteger a){
    int i;
    for(i=AV_INTEGER_SIZE-1; i>=0; i--){
        if(a.v[i])
            return av_log2_16bit(a.v[i]) + 16*i;
    }
    return -1;
}

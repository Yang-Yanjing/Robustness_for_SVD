int av_cmp_i(AVInteger a, AVInteger b){
    int i;
    int v= (int16_t)a.v[AV_INTEGER_SIZE-1] - (int16_t)b.v[AV_INTEGER_SIZE-1];
    if(v) return (v>>16)|1;
    for(i=AV_INTEGER_SIZE-2; i>=0; i--){
        int v= a.v[i] - b.v[i];
        if(v) return (v>>16)|1;
    }
    return 0;
}

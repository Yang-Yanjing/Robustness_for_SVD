AVInteger av_int2i(int64_t a){
    AVInteger out;
    int i;
    for(i=0; i<AV_INTEGER_SIZE; i++){
        out.v[i]= a;
        a>>=16;
    }
    return out;
}

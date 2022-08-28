int64_t av_i2int(AVInteger a){
    int i;
    int64_t out=(int8_t)a.v[AV_INTEGER_SIZE-1];
    for(i= AV_INTEGER_SIZE-2; i>=0; i--){
        out = (out<<16) + a.v[i];
    }
    return out;
}

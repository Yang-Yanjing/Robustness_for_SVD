AVInteger av_mul_i(AVInteger a, AVInteger b){
    AVInteger out;
    int i, j;
    int na= (av_log2_i(a)+16) >> 4;
    int nb= (av_log2_i(b)+16) >> 4;
    memset(&out, 0, sizeof(out));
    for(i=0; i<na; i++){
        unsigned int carry=0;
        if(a.v[i])
            for(j=i; j<AV_INTEGER_SIZE && j-i<=nb; j++){
                carry= (carry>>16) + out.v[j] + a.v[i]*b.v[j-i];
                out.v[j]= carry;
            }
    }
    return out;
}

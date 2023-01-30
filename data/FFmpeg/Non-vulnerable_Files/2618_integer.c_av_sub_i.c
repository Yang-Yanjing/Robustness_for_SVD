AVInteger av_sub_i(AVInteger a, AVInteger b){
    int i, carry=0;
    for(i=0; i<AV_INTEGER_SIZE; i++){
        carry= (carry>>16) + a.v[i] - b.v[i];
        a.v[i]= carry;
    }
    return a;
}

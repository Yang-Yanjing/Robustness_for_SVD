AVInteger av_shr_i(AVInteger a, int s){
    AVInteger out;
    int i;
    for(i=0; i<AV_INTEGER_SIZE; i++){
        unsigned int index= i + (s>>4);
        unsigned int v=0;
        if(index+1<AV_INTEGER_SIZE) v = a.v[index+1]<<16;
        if(index  <AV_INTEGER_SIZE) v+= a.v[index  ];
        out.v[i]= v >> (s&15);
    }
    return out;
}

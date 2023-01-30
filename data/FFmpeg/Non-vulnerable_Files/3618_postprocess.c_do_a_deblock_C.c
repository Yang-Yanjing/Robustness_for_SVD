static av_always_inline void do_a_deblock_C(uint8_t *src, int step,
                                            int stride, const PPContext *c, int mode)
{
    int y;
    const int QP= c->QP;
    const int dcOffset= ((c->nonBQP*c->ppMode.baseDcDiff)>>8) + 1;
    const int dcThreshold= dcOffset*2 + 1;

    src+= step*4; 
    for(y=0; y<8; y++){
        int numEq= 0;
        numEq += ((unsigned)(src[-1*step] - src[0*step] + dcOffset)) < dcThreshold;
        numEq += ((unsigned)(src[ 0*step] - src[1*step] + dcOffset)) < dcThreshold;
        numEq += ((unsigned)(src[ 1*step] - src[2*step] + dcOffset)) < dcThreshold;
        numEq += ((unsigned)(src[ 2*step] - src[3*step] + dcOffset)) < dcThreshold;
        numEq += ((unsigned)(src[ 3*step] - src[4*step] + dcOffset)) < dcThreshold;
        numEq += ((unsigned)(src[ 4*step] - src[5*step] + dcOffset)) < dcThreshold;
        numEq += ((unsigned)(src[ 5*step] - src[6*step] + dcOffset)) < dcThreshold;
        numEq += ((unsigned)(src[ 6*step] - src[7*step] + dcOffset)) < dcThreshold;
        numEq += ((unsigned)(src[ 7*step] - src[8*step] + dcOffset)) < dcThreshold;
        if(numEq > c->ppMode.flatnessThreshold){
            int min, max, x;
            if(src[0] > src[step]){
                max= src[0];
                min= src[step];
            }else{
                max= src[step];
                min= src[0];
            }
            for(x=2; x<8; x+=2){
                if(src[x*step] > src[(x+1)*step]){
                        if(src[x    *step] > max) max= src[ x   *step];
                        if(src[(x+1)*step] < min) min= src[(x+1)*step];
                }else{
                        if(src[(x+1)*step] > max) max= src[(x+1)*step];
                        if(src[ x   *step] < min) min= src[ x   *step];
                }
            }
            if(max-min < 2*QP){
                const int first= FFABS(src[-1*step] - src[0]) < QP ? src[-1*step] : src[0];
                const int last= FFABS(src[8*step] - src[7*step]) < QP ? src[8*step] : src[7*step];
                int sums[10];
                sums[0] = 4*first + src[0*step] + src[1*step] + src[2*step] + 4;
                sums[1] = sums[0] - first       + src[3*step];
                sums[2] = sums[1] - first       + src[4*step];
                sums[3] = sums[2] - first       + src[5*step];
                sums[4] = sums[3] - first       + src[6*step];
                sums[5] = sums[4] - src[0*step] + src[7*step];
                sums[6] = sums[5] - src[1*step] + last;
                sums[7] = sums[6] - src[2*step] + last;
                sums[8] = sums[7] - src[3*step] + last;
                sums[9] = sums[8] - src[4*step] + last;
                if (mode & VISUALIZE) {
                    src[0*step] =
                    src[1*step] =
                    src[2*step] =
                    src[3*step] =
                    src[4*step] =
                    src[5*step] =
                    src[6*step] =
                    src[7*step] = 128;
                }
                src[0*step]= (sums[0] + sums[2] + 2*src[0*step])>>4;
                src[1*step]= (sums[1] + sums[3] + 2*src[1*step])>>4;
                src[2*step]= (sums[2] + sums[4] + 2*src[2*step])>>4;
                src[3*step]= (sums[3] + sums[5] + 2*src[3*step])>>4;
                src[4*step]= (sums[4] + sums[6] + 2*src[4*step])>>4;
                src[5*step]= (sums[5] + sums[7] + 2*src[5*step])>>4;
                src[6*step]= (sums[6] + sums[8] + 2*src[6*step])>>4;
                src[7*step]= (sums[7] + sums[9] + 2*src[7*step])>>4;
            }
        }else{
            const int middleEnergy= 5*(src[4*step] - src[3*step]) + 2*(src[2*step] - src[5*step]);
            if(FFABS(middleEnergy) < 8*QP){
                const int q=(src[3*step] - src[4*step])/2;
                const int leftEnergy=  5*(src[2*step] - src[1*step]) + 2*(src[0*step] - src[3*step]);
                const int rightEnergy= 5*(src[6*step] - src[5*step]) + 2*(src[4*step] - src[7*step]);
                int d= FFABS(middleEnergy) - FFMIN( FFABS(leftEnergy), FFABS(rightEnergy) );
                d= FFMAX(d, 0);
                d= (5*d + 32) >> 6;
                d*= FFSIGN(-middleEnergy);
                if(q>0){
                    d = FFMAX(d, 0);
                    d = FFMIN(d, q);
                }else{
                    d = FFMIN(d, 0);
                    d = FFMAX(d, q);
                }
                if ((mode & VISUALIZE) && d) {
                    d= (d < 0) ? 32 : -32;
                    src[3*step]= av_clip_uint8(src[3*step] - d);
                    src[4*step]= av_clip_uint8(src[4*step] + d);
                    d = 0;
                }
                src[3*step]-= d;
                src[4*step]+= d;
            }
        }
        src += stride;
    }





}

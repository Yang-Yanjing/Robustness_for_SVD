static inline void doHorizDefFilter_C(uint8_t dst[], int stride, const PPContext *c)
{
    int y;
    for(y=0; y<BLOCK_SIZE; y++){
        const int middleEnergy= 5*(dst[4] - dst[3]) + 2*(dst[2] - dst[5]);
        if(FFABS(middleEnergy) < 8*c->QP){
            const int q=(dst[3] - dst[4])/2;
            const int leftEnergy=  5*(dst[2] - dst[1]) + 2*(dst[0] - dst[3]);
            const int rightEnergy= 5*(dst[6] - dst[5]) + 2*(dst[4] - dst[7]);
            int d= FFABS(middleEnergy) - FFMIN( FFABS(leftEnergy), FFABS(rightEnergy) );
            d= FFMAX(d, 0);
            d= (5*d + 32) >> 6;
            d*= FFSIGN(-middleEnergy);
            if(q>0)
            {
                d = FFMAX(d, 0);
                d = FFMIN(d, q);
            }
            else
            {
                d = FFMIN(d, 0);
                d = FFMAX(d, q);
            }
            dst[3]-= d;
            dst[4]+= d;
        }
        dst+= stride;
    }
}

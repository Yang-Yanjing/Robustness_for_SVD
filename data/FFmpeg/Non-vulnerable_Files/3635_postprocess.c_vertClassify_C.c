static inline int vertClassify_C(const uint8_t src[], int stride, const PPContext *c)
{
    if( isVertDC_C(src, stride, c) ){
        return isVertMinMaxOk_C(src, stride, c->QP);
    }else{
        return 2;
    }
}

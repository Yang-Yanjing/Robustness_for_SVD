static inline int horizClassify_C(const uint8_t src[], int stride, const PPContext *c)
{
    if( isHorizDC_C(src, stride, c) ){
        return isHorizMinMaxOk_C(src, stride, c->QP);
    }else{
        return 2;
    }
}

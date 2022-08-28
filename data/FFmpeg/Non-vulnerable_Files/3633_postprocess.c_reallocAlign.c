static void reallocAlign(void **p, int size){
    av_free(*p);
    *p= av_mallocz(size);
}

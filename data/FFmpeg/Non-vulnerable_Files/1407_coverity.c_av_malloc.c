
void *av_malloc(size_t size) {
    int has_memory;
    __coverity_negative_sink__(size);
    if(has_memory)
        return __coverity_alloc__(size);
    else
        return 0;
}

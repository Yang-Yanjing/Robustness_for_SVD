static int get_category(void *ptr){
    AVClass *avc = *(AVClass **) ptr;
    if(    !avc
        || (avc->version&0xFF)<100
        ||  avc->version < (51 << 16 | 59 << 8)
        ||  avc->category >= AV_CLASS_CATEGORY_NB) return AV_CLASS_CATEGORY_NA + 16;
    if(avc->get_category)
        return avc->get_category(ptr) + 16;
    return avc->category + 16;
}

unsigned av_int_list_length_for_size(unsigned elsize,
                                     const void *list, uint64_t term)
{
    unsigned i;
    if (!list)
        return 0;
#define LIST_LENGTH(type) \
    { type t = term, *l = (type *)list; for (i = 0; l[i] != t; i++); }
    switch (elsize) {
    case 1: LIST_LENGTH(uint8_t);  break;
    case 2: LIST_LENGTH(uint16_t); break;
    case 4: LIST_LENGTH(uint32_t); break;
    case 8: LIST_LENGTH(uint64_t); break;
    default: av_assert0(!"valid element size");
    }
    return i;
}

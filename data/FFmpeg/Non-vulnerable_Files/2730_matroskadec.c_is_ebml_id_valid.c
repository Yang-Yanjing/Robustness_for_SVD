static int is_ebml_id_valid(uint32_t id)
{
    
    
    
    unsigned int bits = av_log2(id);
    return id && (bits + 7) / 8 ==  (8 - bits % 8);
}

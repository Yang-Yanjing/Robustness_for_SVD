static int64_t round_to_kag(int64_t position, int kag_size)
{
    
    
    int64_t ret = (position / kag_size) * kag_size;
    return ret == position ? ret : ret + kag_size;
}

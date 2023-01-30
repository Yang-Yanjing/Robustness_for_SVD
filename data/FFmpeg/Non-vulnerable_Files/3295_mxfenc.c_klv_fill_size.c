static unsigned klv_fill_size(uint64_t size)
{
    unsigned pad = KAG_SIZE - (size & (KAG_SIZE-1));
    if (pad < 20) 
        return pad + KAG_SIZE;
    else
        return pad & (KAG_SIZE-1);
}

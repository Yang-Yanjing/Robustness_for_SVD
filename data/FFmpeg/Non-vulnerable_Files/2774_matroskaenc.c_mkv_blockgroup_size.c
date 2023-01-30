static int mkv_blockgroup_size(int pkt_size)
{
    int size = pkt_size + 4;
    size += ebml_num_size(size);
    size += 2;              
    size += 8;              
    size += ebml_num_size(size);
    size += 1;              
    return size;
}

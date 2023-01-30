const char *av_hash_names(int i)
{
    if (i < 0 || i >= NUM_HASHES) return NULL;
    return hashdesc[i].name;
}

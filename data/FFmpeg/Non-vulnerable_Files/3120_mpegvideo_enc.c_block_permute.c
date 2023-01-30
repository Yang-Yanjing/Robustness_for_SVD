static void block_permute(int16_t *block, uint8_t *permutation,
                          const uint8_t *scantable, int last)
{
    int i;
    int16_t temp[64];
    if (last <= 0)
        return;
    
    
    
    for (i = 0; i <= last; i++) {
        const int j = scantable[i];
        temp[j] = block[j];
        block[j] = 0;
    }
    for (i = 0; i <= last; i++) {
        const int j = scantable[i];
        const int perm_j = permutation[j];
        block[perm_j] = temp[j];
    }
}

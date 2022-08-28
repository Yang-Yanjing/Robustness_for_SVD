static void make_bands(int16_t* bands, int start, int stop, int num_bands)
{
    int k, previous, present;
    int base, prod, nz = 0;
    base = (stop << 23) / start;
    while (base < 0x40000000){
        base <<= 1;
        nz++;
    }
    base = fixed_log(base - 0x80000000);
    base = (((base + 0x80) >> 8) + (8-nz)*CONST_LN2) / num_bands;
    base = fixed_exp(base);
    previous = start;
    prod = start << 23;
    for (k = 0; k < num_bands-1; k++) {
        prod = (int)(((int64_t)prod * base + 0x400000) >> 23);
        present = (prod + 0x400000) >> 23;
        bands[k] = present - previous;
        previous = present;
    }
    bands[num_bands-1] = stop - previous;
}

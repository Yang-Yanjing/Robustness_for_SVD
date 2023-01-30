}
static int build_table(VLC *vlc, int table_nb_bits,
                       int nb_codes,
                       const void *bits, int bits_wrap, int bits_size,
                       const void *codes, int codes_wrap, int codes_size,
                       uint32_t code_prefix, int n_prefix)
{
    int i, j, k, n, table_size, table_index, nb, n1, index, code_prefix2;
    uint32_t code;
    int flags = 0;
    VLC_TYPE (*table)[2];
    table_size = 1 << table_nb_bits;
    table_index = alloc_table(vlc, table_size);
#ifdef DEBUG_VLC
    printf("new table index=%d size=%d code_prefix=%x n=%d\n",
           table_index, table_size, code_prefix, n_prefix);
#endif
    if (table_index < 0)
        return -1;
    table = &vlc->table[table_index];
    for(i=0;i<table_size;i++) {
        table[i][1] = 0; //bits
        table[i][0] = -1; //codes
    }
    /* first pass: map codes and compute auxillary table sizes */
    for(i=0;i<nb_codes;i++) {
        GET_DATA(n, bits, i, bits_wrap, bits_size);
        GET_DATA(code, codes, i, codes_wrap, codes_size);
        /* we accept tables with holes */
        if (n <= 0)
            continue;
#if defined(DEBUG_VLC) && 0
        printf("i=%d n=%d code=0x%x\n", i, n, code);
#endif
        /* if code matches the prefix, it is in the table */
        n -= n_prefix;
        if(flags & INIT_VLC_LE)
            code_prefix2= code & (n_prefix>=32 ? 0xffffffff : (uint32_t)(1 << n_prefix)-1);
        else
            code_prefix2= code >> n;
        if (n > 0 && (int)code_prefix2 == (int)code_prefix) {
            if (n <= table_nb_bits) {
                /* no need to add another table */
                j = (code << (table_nb_bits - n)) & (table_size - 1);
                nb = 1 << (table_nb_bits - n);
                for(k=0;k<nb;k++) {
                    if(flags & INIT_VLC_LE)
                        j = (code >> n_prefix) + (k<<n);
#ifdef DEBUG_VLC
                    av_log(NULL, 0, "%4x: code=%d n=%d\n",
                           j, i, n);
#endif
                    if (table[j][1] /*bits*/ != 0) {
                        return -1;
                    }
                    table[j][1] = n; //bits
                    table[j][0] = i; //code
                    j++;
                }
            } else {
                n -= table_nb_bits;
                j = (code >> ((flags & INIT_VLC_LE) ? n_prefix : n)) & ((1 << table_nb_bits) - 1);
#ifdef DEBUG_VLC
                av_log(NULL, 0,"%4x: n=%d (subtable)\n",
                       j, n);
#endif
                /* compute table size */
                n1 = -table[j][1]; //bits
                if (n > n1)
                    n1 = n;
                table[j][1] = -n1; //bits
            }
        }
    }
    /* second pass : fill auxillary tables recursively */
    for(i=0;i<table_size;i++) {
        n = table[i][1]; //bits
        if (n < 0) {
            n = -n;
            if (n > table_nb_bits) {
                n = table_nb_bits;
                table[i][1] = -n; //bits
            }
            index = build_table(vlc, n, nb_codes,
                                bits, bits_wrap, bits_size,
                                codes, codes_wrap, codes_size,
                                (flags & INIT_VLC_LE) ? (code_prefix | (i << n_prefix)) : ((code_prefix << table_nb_bits) | i),
                                n_prefix + table_nb_bits);
            if (index < 0)
                return -1;
            /* note: realloc has been done, so reload tables */
            table = &vlc->table[table_index];
            table[i][0] = index; //code
        }
    }
    return table_index;
}

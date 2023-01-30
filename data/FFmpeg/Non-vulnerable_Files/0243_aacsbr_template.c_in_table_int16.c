static inline int in_table_int16(const int16_t *table, int last_el, int16_t needle)
{
    int i;
    for (i = 0; i <= last_el; i++)
        if (table[i] == needle)
            return 1;
    return 0;
}

static void fill_ones(SwsSlice *s, int n, int is16bit)
{
    int i;
    for (i = 0; i < 4; ++i) {
        int j;
        int size = s->plane[i].available_lines;
        for (j = 0; j < size; ++j) {
            int k;
            int end = is16bit ? n>>1: n;
            
            end += 1;
            if (is16bit)
                for (k = 0; k < end; ++k)
                    ((int32_t*)(s->plane[i].line[j]))[k] = 1<<18;
            else
                for (k = 0; k < end; ++k)
                    ((int16_t*)(s->plane[i].line[j]))[k] = 1<<14;
        }
    }
}

static int estimate_stereo_mode(int32_t *left_ch, int32_t *right_ch, int n)
{
    int i, best;
    int32_t lt, rt;
    uint64_t sum[4];
    uint64_t score[4];
    
    sum[0] = sum[1] = sum[2] = sum[3] = 0;
    for (i = 2; i < n; i++) {
        lt =  left_ch[i] - 2 *  left_ch[i - 1] +  left_ch[i - 2];
        rt = right_ch[i] - 2 * right_ch[i - 1] + right_ch[i - 2];
        sum[2] += FFABS((lt + rt) >> 1);
        sum[3] += FFABS(lt - rt);
        sum[0] += FFABS(lt);
        sum[1] += FFABS(rt);
    }
    
    score[0] = sum[0] + sum[1];
    score[1] = sum[0] + sum[3];
    score[2] = sum[1] + sum[3];
    score[3] = sum[2] + sum[3];
    
    best = 0;
    for (i = 1; i < 4; i++) {
        if (score[i] < score[best])
            best = i;
    }
    return best;
}

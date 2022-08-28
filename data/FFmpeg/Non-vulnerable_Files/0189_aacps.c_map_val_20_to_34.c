static void map_val_20_to_34(INTFLOAT par[PS_MAX_NR_IIDICC])
{
    par[33] =  par[19];
    par[32] =  par[19];
    par[31] =  par[18];
    par[30] =  par[18];
    par[29] =  par[18];
    par[28] =  par[18];
    par[27] =  par[17];
    par[26] =  par[17];
    par[25] =  par[16];
    par[24] =  par[16];
    par[23] =  par[15];
    par[22] =  par[15];
    par[21] =  par[14];
    par[20] =  par[14];
    par[19] =  par[13];
    par[18] =  par[12];
    par[17] =  par[11];
    par[16] =  par[10];
    par[15] =  par[ 9];
    par[14] =  par[ 9];
    par[13] =  par[ 8];
    par[12] =  par[ 8];
    par[11] =  par[ 7];
    par[10] =  par[ 6];
    par[ 9] =  par[ 5];
    par[ 8] =  par[ 5];
    par[ 7] =  par[ 4];
    par[ 6] =  par[ 4];
    par[ 5] =  par[ 3];
    par[ 4] = AAC_HALF_SUM(par[ 2], par[ 3]);
    par[ 3] =  par[ 2];
    par[ 2] =  par[ 1];
    par[ 1] = AAC_HALF_SUM(par[ 0], par[ 1]);
}

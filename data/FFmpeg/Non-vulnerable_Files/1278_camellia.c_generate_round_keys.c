static void generate_round_keys(AVCAMELLIA *cs, uint64_t Kl[2], uint64_t Kr[2], uint64_t Ka[2], uint64_t Kb[2])
{
    int i;
    uint64_t *Kd[4], d[2];
    Kd[0] = Kl;
    Kd[1] = Kr;
    Kd[2] = Ka;
    Kd[3] = Kb;
    cs->Kw[0] = Kl[0];
    cs->Kw[1] = Kl[1];
    if (cs->key_bits == 128) {
        for (i = 0; i < 9; i++) {
            LR128(d, Kd[vars[0][i]], shifts[0][i]);
            cs->K[2*i] = d[0];
            cs->K[2*i+1] = d[1];
        }
        LR128(d, Kd[0], 60);
        cs->K[9] = d[1];
        LR128(d, Kd[2], 30);
        cs->Ke[0] = d[0];
        cs->Ke[1] = d[1];
        LR128(d, Kd[0], 77);
        cs->Ke[2] = d[0];
        cs->Ke[3] = d[1];
        LR128(d, Kd[2], 111);
        cs->Kw[2] = d[0];
        cs->Kw[3] = d[1];
    } else {
        for (i = 0; i < 12; i++) {
            LR128(d, Kd[vars[1][i]], shifts[1][i]);
            cs->K[2*i] = d[0];
            cs->K[2*i+1] = d[1];
        }
        LR128(d, Kd[1], 30);
        cs->Ke[0] = d[0];
        cs->Ke[1] = d[1];
        LR128(d, Kd[0], 60);
        cs->Ke[2] = d[0];
        cs->Ke[3] = d[1];
        LR128(d, Kd[2], 77);
        cs->Ke[4] = d[0];
        cs->Ke[5] = d[1];
        LR128(d, Kd[3], 111);
        cs->Kw[2] = d[0];
        cs->Kw[3] = d[1];
    }
}

static av_always_inline void xbr_filter(const ThreadData *td, int jobnr, int nb_jobs, int n)
{
    int x, y;
    const AVFrame *input = td->in;
    AVFrame *output = td->out;
    const uint32_t *r2y = td->rgbtoyuv;
    const int slice_start = (input->height *  jobnr   ) / nb_jobs;
    const int slice_end   = (input->height * (jobnr+1)) / nb_jobs;
    const int nl = output->linesize[0] >> 2;
    const int nl1 = nl + nl;
    const int nl2 = nl1 + nl;
    for (y = slice_start; y < slice_end; y++) {
        uint32_t *E = (uint32_t *)(output->data[0] + y * output->linesize[0] * n);
        const uint32_t *sa2 = (uint32_t *)(input->data[0] + y * input->linesize[0] - 8); 
        const uint32_t *sa1 = sa2 - (input->linesize[0]>>2); 
        const uint32_t *sa0 = sa1 - (input->linesize[0]>>2); 
        const uint32_t *sa3 = sa2 + (input->linesize[0]>>2); 
        const uint32_t *sa4 = sa3 + (input->linesize[0]>>2); 
        if (y <= 1) {
            sa0 = sa1;
            if (y == 0) {
                sa0 = sa1 = sa2;
            }
        }
        if (y >= input->height - 2) {
            sa4 = sa3;
            if (y == input->height - 1) {
                sa4 = sa3 = sa2;
            }
        }
        for (x = 0; x < input->width; x++) {
            const uint32_t B1 = sa0[2];
            const uint32_t PB = sa1[2];
            const uint32_t PE = sa2[2];
            const uint32_t PH = sa3[2];
            const uint32_t H5 = sa4[2];
            const int pprev = 2 - (x > 0);
            const uint32_t A1 = sa0[pprev];
            const uint32_t PA = sa1[pprev];
            const uint32_t PD = sa2[pprev];
            const uint32_t PG = sa3[pprev];
            const uint32_t G5 = sa4[pprev];
            const int pprev2 = pprev - (x > 1);
            const uint32_t A0 = sa1[pprev2];
            const uint32_t D0 = sa2[pprev2];
            const uint32_t G0 = sa3[pprev2];
            const int pnext = 3 - (x == input->width - 1);
            const uint32_t C1 = sa0[pnext];
            const uint32_t PC = sa1[pnext];
            const uint32_t PF = sa2[pnext];
            const uint32_t PI = sa3[pnext];
            const uint32_t I5 = sa4[pnext];
            const int pnext2 = pnext + 1 - (x >= input->width - 2);
            const uint32_t C4 = sa1[pnext2];
            const uint32_t F4 = sa2[pnext2];
            const uint32_t I4 = sa3[pnext2];
            if (n == 2) {
                E[0]  = E[1]      =     
                E[nl] = E[nl + 1] = PE; 
                FILT2(PE, PI, PH, PF, PG, PC, PD, PB, PA, G5, C4, G0, D0, C1, B1, F4, I4, H5, I5, A0, A1, 0, 1, nl, nl+1);
                FILT2(PE, PC, PF, PB, PI, PA, PH, PD, PG, I4, A1, I5, H5, A0, D0, B1, C1, F4, C4, G5, G0, nl, 0, nl+1, 1);
                FILT2(PE, PA, PB, PD, PC, PG, PF, PH, PI, C1, G0, C4, F4, G5, H5, D0, A0, B1, A1, I4, I5, nl+1, nl, 1, 0);
                FILT2(PE, PG, PD, PH, PA, PI, PB, PF, PC, A0, I5, A1, B1, I4, F4, H5, G5, D0, G0, C1, C4, 1, nl+1, 0, nl);
            } else if (n == 3) {
                E[0]   = E[1]     = E[2]     =     
                E[nl]  = E[nl+1]  = E[nl+2]  =     
                E[nl1] = E[nl1+1] = E[nl1+2] = PE; 
                FILT3(PE, PI, PH, PF, PG, PC, PD, PB, PA, G5, C4, G0, D0, C1, B1, F4, I4, H5, I5, A0, A1, 0, 1, 2, nl, nl+1, nl+2, nl1, nl1+1, nl1+2);
                FILT3(PE, PC, PF, PB, PI, PA, PH, PD, PG, I4, A1, I5, H5, A0, D0, B1, C1, F4, C4, G5, G0, nl1, nl, 0, nl1+1, nl+1, 1, nl1+2, nl+2, 2);
                FILT3(PE, PA, PB, PD, PC, PG, PF, PH, PI, C1, G0, C4, F4, G5, H5, D0, A0, B1, A1, I4, I5, nl1+2, nl1+1, nl1, nl+2, nl+1, nl, 2, 1, 0);
                FILT3(PE, PG, PD, PH, PA, PI, PB, PF, PC, A0, I5, A1, B1, I4, F4, H5, G5, D0, G0, C1, C4, 2, nl+2, nl1+2, 1, nl+1, nl1+1, 0, nl, nl1);
            } else if (n == 4) {
                E[0]   = E[1]     = E[2]     = E[3]     =     
                E[nl]  = E[nl+1]  = E[nl+2]  = E[nl+3]  =     
                E[nl1] = E[nl1+1] = E[nl1+2] = E[nl1+3] =     
                E[nl2] = E[nl2+1] = E[nl2+2] = E[nl2+3] = PE; 
                FILT4(PE, PI, PH, PF, PG, PC, PD, PB, PA, G5, C4, G0, D0, C1, B1, F4, I4, H5, I5, A0, A1, nl2+3, nl2+2, nl1+3, 3, nl+3, nl1+2, nl2+1, nl2, nl1+1, nl+2, 2, 1, nl+1, nl1, nl, 0);
                FILT4(PE, PC, PF, PB, PI, PA, PH, PD, PG, I4, A1, I5, H5, A0, D0, B1, C1, F4, C4, G5, G0, 3, nl+3, 2, 0, 1, nl+2, nl1+3, nl2+3, nl1+2, nl+1, nl, nl1, nl1+1, nl2+2, nl2+1, nl2);
                FILT4(PE, PA, PB, PD, PC, PG, PF, PH, PI, C1, G0, C4, F4, G5, H5, D0, A0, B1, A1, I4, I5, 0, 1, nl, nl2, nl1, nl+1, 2, 3, nl+2, nl1+1, nl2+1, nl2+2, nl1+2, nl+3, nl1+3, nl2+3);
                FILT4(PE, PG, PD, PH, PA, PI, PB, PF, PC, A0, I5, A1, B1, I4, F4, H5, G5, D0, G0, C1, C4, nl2, nl1, nl2+1, nl2+3, nl2+2, nl1+1, nl, 0, nl+1, nl1+2, nl1+3, nl+3, nl+2, 1, 2, 3);
            }
            sa0 += 1;
            sa1 += 1;
            sa2 += 1;
            sa3 += 1;
            sa4 += 1;
            E += n;
        }
    }
}

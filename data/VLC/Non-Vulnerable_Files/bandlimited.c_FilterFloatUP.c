}
static void FilterFloatUP( const float Imp[], const float ImpD[], uint16_t Nwing, float *p_in,
                            float *p_out, uint32_t ui_remainder,
                            uint32_t ui_output_rate, int16_t Inc, int i_nb_channels )
{
    const float *Hp, *Hdp, *End;
    float t, temp;
    uint32_t ui_linear_remainder;
    int i;
    Hp = &Imp[(ui_remainder<<Nhc)/ui_output_rate];
    Hdp = &ImpD[(ui_remainder<<Nhc)/ui_output_rate];
    End = &Imp[Nwing];
    ui_linear_remainder = (ui_remainder<<Nhc) -
                            (ui_remainder<<Nhc)/ui_output_rate*ui_output_rate;
    if (Inc == 1)               /* If doing right wing...              */
    {                           /* ...drop extra coeff, so when Ph is  */
        End--;                  /*    0.5, we don't do too many mult's */
        if (ui_remainder == 0)  /* If the phase is zero...           */
        {                       /* ...then we've already skipped the */
            Hp += Npc;          /*    first sample, so we must also  */
            Hdp += Npc;         /*    skip ahead in Imp[] and ImpD[] */
        }
    }
    while (Hp < End) {
        t = *Hp;                /* Get filter coeff */
                                /* t is now interp'd filter coeff */
        t += *Hdp * ui_linear_remainder / ui_output_rate / Npc;
        for( i = 0; i < i_nb_channels; i++ )
        {
            temp = t;
            temp *= *(p_in+i);  /* Mult coeff by input sample */
            *(p_out+i) += temp; /* The filter output */
        }
        Hdp += Npc;             /* Filter coeff differences step */
        Hp += Npc;              /* Filter coeff step */
        p_in += (Inc * i_nb_channels); /* Input signal step */
    }
}

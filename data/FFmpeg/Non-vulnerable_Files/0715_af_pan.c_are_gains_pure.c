static int are_gains_pure(const PanContext *pan)
{
    int i, j;
    for (i = 0; i < MAX_CHANNELS; i++) {
        int nb_gain = 0;
        for (j = 0; j < MAX_CHANNELS; j++) {
            double gain = pan->gain[i][j];
            

            if (gain != 0. && gain != 1.)
                return 0;
            
            if (gain && nb_gain++)
                return 0;
        }
    }
    return 1;
}

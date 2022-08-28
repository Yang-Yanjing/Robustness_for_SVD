static int yae_adjust_position(ATempoContext *atempo)
{
    const AudioFragment *prev = yae_prev_frag(atempo);
    AudioFragment       *frag = yae_curr_frag(atempo);
    const double prev_output_position =
        (double)(prev->position[1] - atempo->origin[1] + atempo->window / 2);
    const double ideal_output_position =
        (double)(prev->position[0] - atempo->origin[0] + atempo->window / 2) /
        atempo->tempo;
    const int drift = (int)(prev_output_position - ideal_output_position);
    const int delta_max  = atempo->window / 2;
    const int correction = yae_align(frag,
                                     prev,
                                     atempo->window,
                                     delta_max,
                                     drift,
                                     atempo->correlation,
                                     atempo->complex_to_real);
    if (correction) {
        
        frag->position[0] -= correction;
        
        frag->nsamples = 0;
    }
    return correction;
}

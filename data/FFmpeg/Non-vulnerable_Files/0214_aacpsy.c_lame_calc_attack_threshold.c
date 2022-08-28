static float lame_calc_attack_threshold(int bitrate)
{
    
    int lower_range = 12, upper_range = 12;
    int lower_range_kbps = psy_abr_map[12].quality;
    int upper_range_kbps = psy_abr_map[12].quality;
    int i;
    


    for (i = 1; i < 13; i++) {
        if (FFMAX(bitrate, psy_abr_map[i].quality) != bitrate) {
            upper_range = i;
            upper_range_kbps = psy_abr_map[i    ].quality;
            lower_range = i - 1;
            lower_range_kbps = psy_abr_map[i - 1].quality;
            break; 
        }
    }
    
    if ((upper_range_kbps - bitrate) > (bitrate - lower_range_kbps))
        return psy_abr_map[lower_range].st_lrm;
    return psy_abr_map[upper_range].st_lrm;
}

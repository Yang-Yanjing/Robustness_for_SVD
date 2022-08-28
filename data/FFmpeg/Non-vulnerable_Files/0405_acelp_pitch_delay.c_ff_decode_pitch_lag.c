void ff_decode_pitch_lag(int *lag_int, int *lag_frac, int pitch_index,
                         const int prev_lag_int, const int subframe,
                         int third_as_first, int resolution)
{
    
    if (subframe == 0 || (subframe == 2 && third_as_first)) {
        if (pitch_index < 197)
            pitch_index += 59;
        else
            pitch_index = 3 * pitch_index - 335;
    } else {
        if (resolution == 4) {
            int search_range_min = av_clip(prev_lag_int - 5, PITCH_DELAY_MIN,
                                           PITCH_DELAY_MAX - 9);
            
            if (pitch_index < 4) {
                
                pitch_index = 3 * (pitch_index + search_range_min) + 1;
            } else if (pitch_index < 12) {
                
                pitch_index += 3 * search_range_min + 7;
            } else {
                
                pitch_index = 3 * (pitch_index + search_range_min - 6) + 1;
            }
        } else {
            
            pitch_index--;
            if (resolution == 5) {
                pitch_index += 3 * av_clip(prev_lag_int - 10, PITCH_DELAY_MIN,
                                           PITCH_DELAY_MAX - 19);
            } else
                pitch_index += 3 * av_clip(prev_lag_int - 5, PITCH_DELAY_MIN,
                                           PITCH_DELAY_MAX - 9);
        }
    }
    *lag_int  = pitch_index * 10923 >> 15;
    *lag_frac = pitch_index - 3 * *lag_int - 1;
}
}
void ff_decode_pitch_lag(int *lag_int, int *lag_frac, int pitch_index,
                         const int prev_lag_int, const int subframe,
                         int third_as_first, int resolution)
{
    
    if (subframe == 0 || (subframe == 2 && third_as_first)) {
        if (pitch_index < 197)
            pitch_index += 59;
        else
            pitch_index = 3 * pitch_index - 335;
    } else {
        if (resolution == 4) {
            int search_range_min = av_clip(prev_lag_int - 5, PITCH_DELAY_MIN,
                                           PITCH_DELAY_MAX - 9);
            
            if (pitch_index < 4) {
                
                pitch_index = 3 * (pitch_index + search_range_min) + 1;
            } else if (pitch_index < 12) {
                
                pitch_index += 3 * search_range_min + 7;
            } else {
                
                pitch_index = 3 * (pitch_index + search_range_min - 6) + 1;
            }
        } else {
            
            pitch_index--;
            if (resolution == 5) {
                pitch_index += 3 * av_clip(prev_lag_int - 10, PITCH_DELAY_MIN,
                                           PITCH_DELAY_MAX - 19);
            } else
                pitch_index += 3 * av_clip(prev_lag_int - 5, PITCH_DELAY_MIN,
                                           PITCH_DELAY_MAX - 9);
        }
    }

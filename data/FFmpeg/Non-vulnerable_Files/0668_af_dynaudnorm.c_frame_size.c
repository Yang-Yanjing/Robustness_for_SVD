static inline int frame_size(int sample_rate, int frame_len_msec)
{
    const int frame_size = round((double)sample_rate * (frame_len_msec / 1000.0));
    return frame_size + (frame_size % 2);
}

static float iir_filter(int in, float state[2])
{
    float ret;
    ret = 0.7548f * (in - state[0]) + 0.5095f * state[1];
    state[0] = in;
    state[1] = ret;
    return ret;
}

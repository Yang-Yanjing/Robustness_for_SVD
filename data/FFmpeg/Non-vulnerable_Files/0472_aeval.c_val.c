static double val(void *priv, double ch)
{
    EvalContext *eval = priv;
    return eval->channel_values[FFMIN((int)ch, eval->nb_in_channels-1)];
}

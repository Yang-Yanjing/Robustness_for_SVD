static void update_volume(ChanParam *cp, double in)
{
    double delta = in - cp->volume;
    if (delta > 0.0)
        cp->volume += delta * cp->attack;
    else
        cp->volume += delta * cp->decay;
}

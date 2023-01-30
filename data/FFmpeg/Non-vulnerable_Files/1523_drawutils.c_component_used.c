static int component_used(FFDrawContext *draw, int plane, int comp)
{
    return (draw->comp_mask[plane] >> comp) & 1;
}

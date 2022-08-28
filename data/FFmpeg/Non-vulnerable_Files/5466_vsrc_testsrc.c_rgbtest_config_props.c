static int rgbtest_config_props(AVFilterLink *outlink)
{
    TestSourceContext *test = outlink->src->priv;
    ff_fill_rgba_map(test->rgba_map, outlink->format);
    return config_props(outlink);
}

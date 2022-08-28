static const uint8_t *get_graph_color(const EBUR128Context *ebur128, int v, int y)
{
    const int below0  = y > ebur128->y_zero_lu;
    const int reached = y >= v;
    const int line    = ebur128->y_line_ref[y] || y == ebur128->y_zero_lu;
    const int colorid = 4*line + 2*reached + below0;
    return graph_colors + 3*colorid;
}

 */
static void setBarColor(float f_height)
{
    float r, b;
#define BAR_MAX_HEIGHT 4.2
    r = -1.0 + 2 / BAR_MAX_HEIGHT * f_height;
    b = 2.0 - 2 / BAR_MAX_HEIGHT * f_height;
#undef BAR_MAX_HEIGHT
    /* Test the ranges. */
    r = r > 1.0 ? 1.0 : r;
    b = b > 1.0 ? 1.0 : b;
    r = r < 0.0 ? 0.0 : r;
    b = b < 0.0 ? 0.0 : b;
    /* Set the bar color. */
    glColor4f(r, 0.0, b, 1.0);
}

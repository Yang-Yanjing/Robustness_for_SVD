 */
static void drawBars(float heights[])
{
    glPushMatrix();
    glTranslatef(-2.0, 0.0, 0.0);
    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_NORMAL_ARRAY);
    float w = SPECTRUM_WIDTH / NB_BANDS;
    for (unsigned i = 0; i < NB_BANDS; ++i)
    {
        glPushMatrix();
        glScalef(1.0, heights[i], 1.0);
        setBarColor(heights[i]);
        drawBar();
        glPopMatrix();
        glTranslatef(w, 0.0, 0.0);
    }
    glDisableClientState(GL_VERTEX_ARRAY);
    glDisableClientState(GL_NORMAL_ARRAY);
    glPopMatrix();
}

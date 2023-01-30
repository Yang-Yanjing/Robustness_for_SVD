  **/
static void initOpenGLScene(void)
{
    glEnable(GL_CULL_FACE);
    glEnable(GL_DEPTH_TEST);
    glDepthMask(GL_TRUE);
    glMatrixMode(GL_PROJECTION);
    glFrustum(-1.0f, 1.0f, -1.0f, 1.0f, 0.5f, 10.0f);
    glMatrixMode(GL_MODELVIEW);
    glTranslatef(0.0, -2.0, -2.0);
    // Init the light.
    glEnable(GL_LIGHTING);
    glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);
    glEnable(GL_COLOR_MATERIAL);
    glEnable(GL_LIGHT0);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, lightZeroColor);
    glLightfv(GL_LIGHT0, GL_POSITION, lightZeroPosition);
    glShadeModel(GL_SMOOTH);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

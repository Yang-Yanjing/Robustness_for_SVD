static pthread_mutex_t xlib_lock = PTHREAD_MUTEX_INITIALIZER;
int (*XSetErrorHandler (int (*handler) (Display *, XErrorEvent *)))
     (Display *, XErrorEvent *)
{
    if (override)
    {
        int (*ret) (Display *, XErrorEvent *);
        pthread_mutex_lock (&xlib_lock);
        LOG("Error", "%p", handler);
        ret = CALL(XSetErrorHandler, handler);
        pthread_mutex_unlock (&xlib_lock);
        return ret;
    }
    return CALL(XSetErrorHandler, handler);
}

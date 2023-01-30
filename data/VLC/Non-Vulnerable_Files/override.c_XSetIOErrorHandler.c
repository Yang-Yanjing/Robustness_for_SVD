}
int (*XSetIOErrorHandler (int (*handler) (Display *))) (Display *)
{
    if (override)
    {
        int (*ret) (Display *);
        pthread_mutex_lock (&xlib_lock);
        LOG("Error", "%p", handler);
        ret = CALL(XSetIOErrorHandler, handler);
        pthread_mutex_unlock (&xlib_lock);
        return ret;
    }
    return CALL(XSetIOErrorHandler, handler);
}

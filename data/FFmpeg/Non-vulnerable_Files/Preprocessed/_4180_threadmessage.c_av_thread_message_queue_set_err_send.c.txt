func	1:0	10:0	0	void	av_thread_message_queue_set_err_send
params	1:41	0:0	1	
param	1:42	1:66	2	AVThreadMessageQueue *	mq
param	2:42	2:49	2	int	err
stmnts	5:4	8:35	1	
call	5:4	5:32	2	pthread_mutex_lock
arg	5:23	5:32	3	&mq->lock
op	5:23	0:0	4	&
water	5:24	0:0	4	mq
op	5:26	0:0	4	->
water	5:28	0:0	4	lock
water	5:33	0:0	2	;
water	6:4	0:0	2	mq
op	6:6	0:0	2	->
water	6:8	0:0	2	err_send
op	6:17	0:0	2	=
water	6:19	0:0	2	err
water	6:22	0:0	2	;
call	7:4	7:36	2	pthread_cond_broadcast
arg	7:27	7:36	3	&mq->cond
op	7:27	0:0	4	&
water	7:28	0:0	4	mq
op	7:30	0:0	4	->
water	7:32	0:0	4	cond
water	7:37	0:0	2	;
call	8:4	8:34	2	pthread_mutex_unlock
arg	8:25	8:34	3	&mq->lock
op	8:25	0:0	4	&
water	8:26	0:0	4	mq
op	8:28	0:0	4	->
water	8:30	0:0	4	lock
water	8:35	0:0	2	;

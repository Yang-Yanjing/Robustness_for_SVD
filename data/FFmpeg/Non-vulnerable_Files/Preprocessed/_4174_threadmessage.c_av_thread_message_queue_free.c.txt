func	1:0	11:0	0	void	av_thread_message_queue_free
params	1:33	0:0	1	
param	1:34	1:59	2	AVThreadMessageQueue * *	mq
stmnts	4:4	9:4	1	
if	4:4	9:4	2	(* mq)
cond	4:8	4:9	3	* mq
op	4:8	0:0	4	*
water	4:9	0:0	4	mq
stmts	4:13	9:4	3	
water	4:13	0:0	4	{
call	5:8	5:34	4	av_fifo_freep
arg	5:22	5:34	5	&(*mq)->fifo
op	5:22	0:0	6	&
water	5:23	0:0	6	(
op	5:24	0:0	6	*
water	5:25	0:0	6	mq
water	5:27	0:0	6	)
op	5:28	0:0	6	->
water	5:30	0:0	6	fifo
water	5:35	0:0	4	;
call	6:8	6:41	4	pthread_cond_destroy
arg	6:29	6:41	5	&(*mq)->cond
op	6:29	0:0	6	&
water	6:30	0:0	6	(
op	6:31	0:0	6	*
water	6:32	0:0	6	mq
water	6:34	0:0	6	)
op	6:35	0:0	6	->
water	6:37	0:0	6	cond
water	6:42	0:0	4	;
call	7:8	7:42	4	pthread_mutex_destroy
arg	7:30	7:42	5	&(*mq)->lock
op	7:30	0:0	6	&
water	7:31	0:0	6	(
op	7:32	0:0	6	*
water	7:33	0:0	6	mq
water	7:35	0:0	6	)
op	7:36	0:0	6	->
water	7:38	0:0	6	lock
water	7:43	0:0	4	;
call	8:8	8:19	4	av_freep
arg	8:17	8:19	5	mq
water	8:17	0:0	6	mq
water	8:20	0:0	4	;
water	9:4	0:0	4	}

#include "msg_queue.h"

#include <assert.h>
#include <errno.h>
#include <pthread.h>
#include <stdbool.h>
#include <sys/time.h>
#include <time.h>

#include "circbuf.h"
#include "log.h"

DECLARE_CIRCBUF(msg_buf, Message);

struct msg_queue {
	struct msg_buf	queue;
	pthread_mutex_t read_lock;
	pthread_cond_t	read_cond;
	pthread_mutex_t write_lock;
	pthread_cond_t	write_cond;
	bool			open;
};

void msg_queue_open(MsgQueue *queue) {
	queue->open = 1;
}

int msg_queue_is_open(MsgQueue *queue) {
	return queue->open;
}

void msg_queue_close(MsgQueue *queue) {
	queue->open = 0;
}

int msg_queue_empty(MsgQueue *queue) {
	return circbuf_empty(&queue->queue);
}

int msg_queue_full(MsgQueue *queue) {
	return circbuf_full(&queue->queue);
}

MsgQueue *msg_queue_create(unsigned int capacity) {
	struct msg_queue *msg_queue = malloc(sizeof(struct msg_queue));
	if (!msg_queue || !circbuf_alloc(&msg_queue->queue, Message, capacity)) {
		log_error("Failed to allocate message queue");
		if (msg_queue)
			free(msg_queue);
		circbuf_free(&msg_queue->queue);

		return NULL;
	}
	msg_queue->open = 0;
	pthread_mutex_init(&msg_queue->read_lock, NULL);
	pthread_mutex_init(&msg_queue->write_lock, NULL);
	pthread_cond_init(&msg_queue->read_cond, NULL);
	pthread_cond_init(&msg_queue->write_cond, NULL);

	return msg_queue;
}

void msg_queue_destroy(MsgQueue *queue) {
	if (!queue) {
		return;
	}
	msg_queue_close(queue);
	pthread_cond_broadcast(&queue->read_cond);
	pthread_cond_broadcast(&queue->write_cond);
	pthread_mutex_destroy(&queue->read_lock);
	pthread_mutex_destroy(&queue->write_lock);
	pthread_cond_destroy(&queue->read_cond);
	pthread_cond_destroy(&queue->write_cond);
	circbuf_free(&queue->queue);
	free(queue);
}

int msg_queue_push_wait(MsgQueue *queue, Message *msg) {
	assert(queue != NULL);
	assert(msg != NULL);
	int ret = 0;
	pthread_mutex_lock(&queue->write_lock);

	while (queue->open && circbuf_full(&queue->queue))
		pthread_cond_wait(&queue->write_cond, &queue->write_lock);

	ret = circbuf_put(&queue->queue, *msg);
	pthread_cond_signal(&queue->read_cond);
	pthread_mutex_unlock(&queue->write_lock);

	return ret;
}

int msg_queue_push_timeout(MsgQueue *queue, Message *msg, unsigned long timeout_ms) {
	assert(queue != NULL);
	assert(msg != NULL);
	struct timeval	tv;
	struct timespec timeout;
	gettimeofday(&tv, NULL);
	timeout.tv_sec	= tv.tv_sec + timeout_ms / 1000;
	timeout.tv_nsec = (tv.tv_usec + (timeout_ms % 1000) * 1000) * 1000;

	int ret = 0;
	pthread_mutex_lock(&queue->write_lock);

	while (queue->open && circbuf_full(&queue->queue)) {
		int lockstatus = pthread_cond_timedwait(&queue->write_cond, &queue->write_lock, &timeout);
		if (lockstatus == ETIMEDOUT) {
			pthread_mutex_unlock(&queue->write_lock);
			return 0;
		}
	}

	ret = circbuf_put(&queue->queue, *msg);
	pthread_cond_signal(&queue->read_cond);
	pthread_mutex_unlock(&queue->write_lock);

	return ret;
}

int msg_queue_try_pop(MsgQueue *queue, Message *msg_out) {
	assert(queue != NULL);
	int ret = 0;
	pthread_mutex_lock(&queue->read_lock);
	if (queue->open)
		ret = circbuf_get(&queue->queue, msg_out);
	if (ret)
		pthread_cond_signal(&queue->write_cond);
	pthread_mutex_unlock(&queue->read_lock);

	return ret;
}

int msg_queue_pop_wait(MsgQueue *queue, Message *msg_out) {
	assert(queue != NULL);
	int ret = 0;
	pthread_mutex_lock(&queue->read_lock);

	while (queue->open && circbuf_empty(&queue->queue))
		pthread_cond_wait(&queue->read_cond, &queue->read_lock);

	ret = circbuf_get(&queue->queue, msg_out);
	pthread_cond_signal(&queue->write_cond);
	pthread_mutex_unlock(&queue->read_lock);
	return ret;
}

int msg_queue_pop_timeout(MsgQueue *queue, Message *msg_out, unsigned long timeout_ms) {
	assert(queue != NULL);
	struct timeval	tv;
	struct timespec timeout;
	gettimeofday(&tv, NULL);
	timeout.tv_sec	= tv.tv_sec + timeout_ms / 1000;
	timeout.tv_nsec = (tv.tv_usec + (timeout_ms % 1000) * 1000) * 1000;

	int ret = 0;
	pthread_mutex_lock(&queue->read_lock);

	while (queue->open && circbuf_empty(&queue->queue)) {
		int lockstatus = pthread_cond_timedwait(&queue->read_cond, &queue->read_lock, &timeout);
		if (lockstatus == ETIMEDOUT) {
			pthread_mutex_unlock(&queue->read_lock);
			return 0;
		}
	}

	ret = circbuf_get(&queue->queue, msg_out);
	pthread_cond_signal(&queue->write_cond);
	pthread_mutex_unlock(&queue->read_lock);
	return ret;
}

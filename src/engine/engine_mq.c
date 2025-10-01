#include "engine_mq.h"

#include <errno.h>
#include <threads.h>

#include "circbuf.h"
#include "engine_types.h"
#include "log.h"

#define ENGINE_MSG_QUEUE_SIZE (1 << 6)

DECLARE_CIRCBUF(eng_msgq, EngineMsg);

struct eng_mq {
	struct eng_msgq queue;
	mtx_t			read_lock;
	cnd_t			read_cond;
	mtx_t			write_lock;
	cnd_t			write_cond;
	bool			open;
};

static EngineMsg	 eng_msgq_buf[ENGINE_MSG_QUEUE_SIZE];
static struct eng_mq eng_mq = {0};

int engmq_init(void) {
	if (cnd_init(&eng_mq.read_cond) == thrd_error) {
		return -1;
	}
	if (cnd_init(&eng_mq.write_cond) == thrd_error) {
		cnd_destroy(&eng_mq.read_cond);
		return -1;
	}
	if (mtx_init(&eng_mq.read_lock, mtx_plain) == thrd_error) {
		cnd_destroy(&eng_mq.write_cond);
		cnd_destroy(&eng_mq.read_cond);
		return -1;
	}
	if (mtx_init(&eng_mq.write_lock, mtx_plain) == thrd_error) {
		mtx_destroy(&eng_mq.read_lock);
		cnd_destroy(&eng_mq.write_cond);
		cnd_destroy(&eng_mq.read_cond);
		return -1;
	}
	circbuf_init(&eng_mq.queue, eng_msgq_buf, ENGINE_MSG_QUEUE_SIZE);
	eng_mq.open = true;
	return 0;
}

void engmq_destroy(void) {
	eng_mq.open = false;
	cnd_broadcast(&eng_mq.read_cond);
	cnd_broadcast(&eng_mq.write_cond);

	cnd_destroy(&eng_mq.read_cond);
	cnd_destroy(&eng_mq.write_cond);
	mtx_destroy(&eng_mq.read_lock);
	mtx_destroy(&eng_mq.write_lock);
}

bool engmq_is_open(void) {
	return eng_mq.open;
}

static int __push_msg_wait(struct engine_msg msg) {
	log_trace("engmq_push_wait");
	mtx_lock(&eng_mq.write_lock);
	if (!engmq_is_open()) {
		mtx_unlock(&eng_mq.write_lock);
		log_error("engmq not open");
		return -EACCES;
	}
	while (circbuf_full(&eng_mq.queue)) {
		log_trace("engine mq full, waiting");
		if (cnd_wait(&eng_mq.write_cond, &eng_mq.write_lock) == thrd_error) {
			log_error("push msg wait failed");
			mtx_unlock(&eng_mq.write_lock);
			return -EBUSY;
		}
	}

	int ret = circbuf_put(&eng_mq.queue, msg);
	if (ret == 0) {
		log_error("push msg failed");
		ret = -EAGAIN;
	}

	cnd_signal(&eng_mq.read_cond);
	mtx_unlock(&eng_mq.write_lock);
	log_trace("engmq_push_wait done");
	return ret;
}

int engmq_send_uci_msg(struct uci_msg *msg) {
	log_trace("engmq_send_uci_msg");
	struct engine_msg eng_msg = {MSG_UCI, .payload.uci = *msg};
	return __push_msg_wait(eng_msg);
}

int engmq_send_search_msg(struct search_msg *msg) {
	log_trace("engmq_send_search_msg");
	struct engine_msg eng_msg = {MSG_UCI, .payload.search = *msg};
	return __push_msg_wait(eng_msg);
}

int engmq_receive(struct engine_msg *msg_out) {
	log_trace("engmq_receive");
	mtx_lock(&eng_mq.read_lock);
	while (eng_mq.open && circbuf_empty(&eng_mq.queue)) {
		log_trace("engmq_receive wait");
		if (cnd_wait(&eng_mq.read_cond, &eng_mq.read_lock) == thrd_error) {
			log_trace("engmq_receive wait error");
			mtx_unlock(&eng_mq.read_lock);
			return -EBUSY;
		}
	}
	int ret = circbuf_get(&eng_mq.queue, msg_out);
	if (ret == 0) {
		ret = -EAGAIN;
	}
	cnd_signal(&eng_mq.write_cond);
	mtx_unlock(&eng_mq.read_lock);
	log_trace("engmq_receive ret %d", ret);
	return ret;
}

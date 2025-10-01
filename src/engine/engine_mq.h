#ifndef ENGINE_MQ_H
#define ENGINE_MQ_H

struct uci_msg;
struct search_msg;
struct engine_msg;

int	 engmq_init(void);
void engmq_destroy(void);

int engmq_send_uci_msg(struct uci_msg *msg);
int engmq_send_search_msg(struct search_msg *msg);

int engmq_receive(struct engine_msg *msg_out);

#endif	// ENGINE_MQ_H

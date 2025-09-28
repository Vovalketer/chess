#ifndef MSG_QUEUE_H
#define MSG_QUEUE_H

typedef struct msg_queue MsgQueue;

typedef enum {
	MSG_UCI,
	MSG_SEARCH,
} MessageType;

typedef enum uci_msg_type {
	MSG_UCI_NONE,
	MSG_UCI_QUIT,
	MSG_UCI_UCI,
	MSG_UCI_ISREADY,
	MSG_UCI_POSITION,
	MSG_UCI_UCINEWGAME,
	MSG_UCI_GO,
	MSG_UCI_STOP,
	MSG_UCI_SETOPTION,
	MSG_UCI_DEBUG,
	MSG_UCI_PRINT
} UciMsgType;

typedef enum search_msg_type {
	MSG_SEARCH_NONE,
	MSG_SEARCH_SEARCH_INFO,
	MSG_SEARCH_STOP,
} SearchMsgType;

typedef struct message {
	MessageType	 type;
	unsigned int subtype;
	void		*payload;
	void (*free_payload)(void *payload, MessageType type, unsigned int subtype);
} Message;

MsgQueue *msg_queue_create(unsigned int capacity);
void	  msg_queue_destroy(MsgQueue *queue);

void msg_queue_open(MsgQueue *queue);
int	 msg_queue_is_open(MsgQueue *queue);
void msg_queue_close(MsgQueue *queue);

int msg_queue_empty(MsgQueue *queue);
int msg_queue_full(MsgQueue *queue);

int msg_queue_try_push(MsgQueue *queue, Message *msg);
int msg_queue_push_wait(MsgQueue *queue, Message *msg);
int msg_queue_push_timeout(MsgQueue *queue, Message *msg, unsigned long timeout_ms);

int msg_queue_try_pop(MsgQueue *queue, Message **msg);
int msg_queue_pop_wait(MsgQueue *queue, Message **msg);
int msg_queue_pop_timeout(MsgQueue *queue, Message **msg, unsigned long timeout_ms);

#endif	// MSG_QUEUE_H

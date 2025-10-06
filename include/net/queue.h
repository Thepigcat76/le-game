#pragma once

#include "../../include/net/packet.h"
#include <pthread.h>
#include <stdbool.h>

struct _queue_node {
  Packet packet;
  struct _queue_node *next;
};

typedef struct {
  struct _queue_node *head;
  struct _queue_node *tail;
  pthread_mutex_t lock;
  pthread_cond_t cond;
} PacketQueue;

void queue_init(PacketQueue *queue);

void queue_push(PacketQueue *queue, Packet packet);

bool queue_pop(PacketQueue *queue, Packet *out);

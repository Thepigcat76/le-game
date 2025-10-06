#include "../../include/net/queue.h"

void queue_init(PacketQueue *queue) {
  queue->head = queue->tail = NULL;
  pthread_mutex_init(&queue->lock, NULL);
  pthread_cond_init(&queue->cond, NULL);
}

void queue_push(PacketQueue *queue, Packet packet) {
  struct _queue_node *node = malloc(sizeof(struct _queue_node));
  node->packet = packet;
  node->next = NULL;

  pthread_mutex_lock(&queue->lock);
  {
    if (queue->tail) {
      queue->tail->next = node;
      queue->tail = node;
    } else {
      queue->head = queue->tail = node;
    }

    // Wake up main thread if it's waiting
    pthread_cond_signal(&queue->cond);
  }
  pthread_mutex_unlock(&queue->lock);
}

bool queue_pop(PacketQueue *queue, Packet *out) {
  pthread_mutex_lock(&queue->lock);

  if (!queue->head) {
    pthread_mutex_unlock(&queue->lock);
    return false;
  }

  struct _queue_node *node = queue->head;
  *out = node->packet;
  queue->head = node->next;
  if (!queue->head)
    queue->tail = NULL;

  free(node);
  pthread_mutex_unlock(&queue->lock);
  return true;
}

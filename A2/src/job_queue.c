#include <stdlib.h>
#include <stdio.h>
#include <assert.h>

#include "job_queue.h"

struct workerArgs
{
  char const *needle;
  char *path;
};


int job_queue_init(struct job_queue *job_queue, int capacity) {
  job_queue->job = calloc(capacity, sizeof(void *));
  assert(pthread_cond_init(&job_queue->popped, NULL) == 0);
  assert(pthread_cond_init(&job_queue->pushed, NULL) == 0);
  assert(pthread_mutex_init(&job_queue->queue_mutex, NULL) == 0);
  job_queue->capacity = capacity;
  job_queue->head = 0;
  job_queue->tail = 0;
  job_queue->count = 0;
  job_queue->destroyed = 0;
  return 0;
}

int job_queue_destroy(struct job_queue *job_queue) {
  job_queue->destroyed = 1;
  pthread_mutex_lock(&job_queue->queue_mutex);

  while (job_queue->count != 0) {
    pthread_cond_wait(&job_queue->popped, &job_queue->queue_mutex);
  }

  pthread_cond_broadcast(&job_queue->pushed);
  free(job_queue->job);
  pthread_mutex_unlock(&job_queue->queue_mutex);
  return 0;
}

int job_queue_push(struct job_queue *job_queue, void *data) {

  pthread_mutex_lock(&job_queue->queue_mutex);
  if (job_queue->destroyed == 1) {
    pthread_mutex_unlock(&job_queue->queue_mutex);
    return 1;
  }

  while (job_queue->count + 1 > job_queue->capacity) {
    if (job_queue->destroyed == 1) {
      pthread_mutex_unlock(&job_queue->queue_mutex);
      return 1;
    }
    pthread_cond_wait(&job_queue->popped, &job_queue->queue_mutex);
  }


  job_queue->job[job_queue->tail] = data;

  job_queue->tail = (job_queue->tail + 1) % job_queue->capacity;

  job_queue->count++;

  pthread_cond_broadcast(&job_queue->pushed);
  pthread_mutex_unlock(&job_queue->queue_mutex);

  return 0;

}

int job_queue_pop(struct job_queue *job_queue, void **data) {

  pthread_mutex_lock(&job_queue->queue_mutex);
  while (job_queue->count <= 0) {
    if (job_queue->destroyed != 0) {
      pthread_mutex_unlock(&job_queue->queue_mutex);
      return 1;
    }
    pthread_cond_wait(&job_queue->pushed, &job_queue->queue_mutex);
  }

  *data = job_queue->job[job_queue->head];
  job_queue->count--;
  job_queue->head = (job_queue->head + 1) % job_queue->capacity;

  pthread_cond_signal(&job_queue->popped);
  pthread_mutex_unlock(&job_queue->queue_mutex);

  return 0;
}


#include "cias_circle_queue.h"


// 初始化循环队列
bool cias_init_circle_queue(CircularQueue *queue, uint32_t queue_len, uint32_t item_size)
{
    queue->pdata = pvPortMalloc(queue_len*item_size);
    if (!queue->pdata)
    {
        ci_logerr(CI_LOG_ERROR, "queue->pdata malloc error\r\n");
        return false;
    }
    queue->queue_len = queue_len;
    queue->item_size = item_size;
    queue->queue_size = queue_len*item_size;
    queue->front = 0;
    queue->rear = 0;
    return true;
}

// 判断队列是否为空
bool cias_queue_is_empty(CircularQueue *queue)
{
    return queue->front == queue->rear;
}

// 判断队列是否已满
bool cias_queue_is_full(CircularQueue *queue)
{
    return (queue->rear + 1) % queue->queue_size == queue->front;
}

// 数据入队操作
bool cias_data_in_queue(CircularQueue *queue, uint8_t value)
{
    if (cias_queue_is_full(queue))
    {
        mprintf("Queue is full, cannot cias_data_in_queue.\n");
        return false;
    }
    queue->pdata[queue->rear] = value;
    queue->rear = (queue->rear + 1) % queue->queue_size;
    return true;
}

// 出队操作
bool cias_data_out_queue(CircularQueue *queue, int *value)
{
    if (cias_queue_is_empty(queue))
    {
        mprintf("Queue is empty, cannot cias_data_out_queue.\n");
        return false;
    }
    *value = queue->pdata[queue->front];
    queue->front = (queue->front + 1) % queue->queue_size;
    return true;
}

// 获取队头元素
bool getFront(CircularQueue *queue, int *value)
{
    if (cias_queue_is_empty(queue))
    {
        mprintf("Queue is empty, no front element.\n");
        return false;
    }
    *value = queue->pdata[queue->front];
    return true;
}

// 打印队列元素
void cias_print_queue(CircularQueue *queue)
{
    if (cias_queue_is_empty(queue))
    {
        mprintf("Queue is empty.\n");
        return;
    }
    int i = queue->front;
    while (i != queue->rear)
    {
        mprintf("%x ", queue->pdata[i]);
        i = (i + 1) % queue->queue_size;
    }
    mprintf("\n");
}
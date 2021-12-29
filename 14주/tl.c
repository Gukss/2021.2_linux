#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>

typedef struct node {
    int data;
    struct node *next;
} Node_t;

typedef struct list {
    Node_t *head;
    Node_t *tail;
    pthread_mutex_t lock;
}List_t;

char **ptr;
List_t total;
int counter = 0;

void print(List_t *t)
{
    Node_t *cur = t->head;
    while(cur->next != NULL)
    {
        printf("%d\n", cur->data);
        cur = cur->next;
    }
    printf("%d\n", cur->data);

    printf("%d\n", t->head->data);
}

void List_append(Node_t *h, List_t *t)
{
    if(t->head == NULL)
    {
        t->head = h;
        Node_t *cur = t->head;
        while(cur->next != NULL)
        {
            //printf("[1] %d\n", cur->data);
            cur = cur->next;
        }
        //printf("[1] %d\n", cur->data);
        t->tail = cur;
    }
    else
    {
        t->tail->next = h;
        Node_t *cur = t->head;
        while(cur->next != NULL)
        {
            //printf("[2] %d\n", cur->data);
            cur = cur->next;
        }
        //printf("[2] %d\n", cur->data);
        t->tail = cur;
    }
    //printf("----------------\n");
}

void append(Node_t *list, int data)
{
    if(list->data == 0)
    {
        list->data = data;
        list->next = NULL;
    }
    else if(list->next == NULL)
    {
        Node_t *newNode = malloc(sizeof(Node_t));
        newNode->data = data;
        newNode->next = NULL;
        list->next = newNode;
    }
    else
    {
        Node_t *cur = list;
        while(cur->next != NULL)
        {
            cur = cur->next;
        }
        Node_t *newNode = malloc(sizeof(Node_t));
        newNode->data = data;
        newNode->next = NULL;
        cur->next = newNode;
    }
}

void *make_piece(void *arg)
{
    for(int j = 0; j < atoi(ptr[3]); j++)
    {
        Node_t *list = malloc(sizeof(Node_t));
        for (int i = 0; i < atoi(ptr[2]); i++)
        {
            //pthread_mutex_lock(&total.lock);
            append(list, --counter);
            //pthread_mutex_unlock(&total.lock);
        }
        Node_t *cur = list;
        while (cur->next != NULL)
        {
            //printf("%d\n", cur->data);
            cur = cur->next;
        }

        //pthread_mutex_lock(&total.lock);
        List_append(list, &total);
        //pthread_mutex_unlock(&total.lock);
    }

    print(&total);
    printf("%d\n", *(int *)arg);

    pthread_exit(NULL);
}

int main(int argc, char *argv[])
{
    //mutex 초기화
    pthread_mutex_init(&total.lock, NULL);
    total.head = NULL;
    total.tail = NULL;
    int rc;

    pthread_t *tid;
    ptr = argv;
    
    tid = (pthread_t *)malloc(sizeof(pthread_t) * atoi(ptr[1]));

    counter = atoi(ptr[1]) * atoi(ptr[2]) * atoi(ptr[3]);
    int flag = counter;
    
    for(int i = 0;i<atoi(argv[1]); i++)
    {
        rc = pthread_create(&tid[i], NULL, make_piece, (void*)&flag);
        if(rc)
        {
            printf("error\n");
        }
    }

    pthread_exit(NULL);
}
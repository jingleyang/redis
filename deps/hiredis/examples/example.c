#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <hiredis.h>

int main(int argc, char **argv) {
    unsigned int j;
    redisContext *c;
    redisReply *reply;
    const char *hostname = (argc > 1) ? argv[1] : "127.0.0.1";
    int port = (argc > 2) ? atoi(argv[2]) : 6379;

    struct timeval timeout = { 1, 500000 }; // 1.5 seconds
    c = redisConnectWithTimeout(hostname, port, timeout);
    if (c == NULL || c->err) {
        if (c) {
            printf("Connection error: %s\n", c->errstr);
            redisFree(c);
        } else {
            printf("Connection error: can't allocate redis context\n");
        }
        exit(1);
    }

    /* PING server */
    reply = redisCommand(c,"PING");
    if (reply) {
        printf("PING: %s\n", reply->str);
        freeReplyObject(reply);
    } else {
        printf("PING: There was an error in performing the request.\n");
    }

    /* Set a key */
    reply = redisCommand(c,"SET %s %s", "foo", "hello world");
    if (reply) {
        printf("SET: %s\n", reply->str);
        freeReplyObject(reply);
    } else {
        printf("SET: There was an error in performing the request.\n");
    }

    /* Set a key using binary safe API */
    reply = redisCommand(c,"SET %b %b", "bar", (size_t) 3, "hello", (size_t) 5);
    if (reply) {
        printf("SET (binary API): %s\n", reply->str);
        freeReplyObject(reply);
    } else {
        printf("SET (binary API): There was an error in performing the request.\n");
    }

    /* Try a GET and two INCR */
    reply = redisCommand(c,"GET foo");
    if (reply) {
        printf("GET foo: %s\n", reply->str);
        freeReplyObject(reply);
    } else {
        printf("GET foo: There was an error in performing the request.\n");
    }

    reply = redisCommand(c,"INCR counter");
    if (reply) {
        printf("INCR counter: %lld\n", reply->integer);
        freeReplyObject(reply);
    } else {
        printf("INCR counter: There was an error in performing the request.\n");
    }

    /* again ... */
    reply = redisCommand(c,"INCR counter");
    if (reply) {
        printf("INCR counter: %lld\n", reply->integer);
        freeReplyObject(reply);
    } else {
        printf("INCR counter: There was an error in performing the request.\n");
    }

    /* Create a list of numbers, from 0 to 9 */
    reply = redisCommand(c,"DEL mylist");
    if (reply) {
        freeReplyObject(reply);
    } else {
        printf("DEL: There was an error in performing the request.\n");
    }

    for (j = 0; j < 10; j++) {
        char buf[64];

        snprintf(buf,64,"%d",j);
        reply = redisCommand(c,"LPUSH mylist element-%s", buf);
        if (reply) {
            freeReplyObject(reply);
        } else {
            printf("LPUSH: There was an error in performing the request.\n");
        }
    }

    /* Let's check what we have inside the list */
    reply = redisCommand(c,"LRANGE mylist 0 -1");
    if (reply) {
        if (reply->type == REDIS_REPLY_ARRAY) {
            for (j = 0; j < reply->elements; j++) {
                printf("%u) %s\n", j, reply->element[j]->str);
            }
        }
        freeReplyObject(reply);
    }
    /* Disconnects and frees the context */
    redisFree(c);

    return 0;
}

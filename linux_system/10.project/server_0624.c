#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <errno.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <signal.h>
#include <time.h>

#define PORT 10000
#define MAX_CLIENTS 2
#define SEQ_LEN 5

// 클라이언트 정보 구조체
typedef struct {
    int sockfd;
    int player_id;
} client_info_t;

// 게임 상태 구조체
typedef struct {
    int scores[MAX_CLIENTS];
    int current_round;
    pthread_mutex_t lock;
} game_state_t;

game_state_t game = {{0}, 0, PTHREAD_MUTEX_INITIALIZER};
client_info_t *clients[MAX_CLIENTS] = {NULL};

void flush_socket(int sock) {
    char tmp[256]; int n;
    while ((n = recv(sock, tmp, sizeof(tmp), MSG_DONTWAIT)) > 0) { }
}

void wait_for_ready() {
    char buf[64];
    for (int i = 0; i < MAX_CLIENTS; i++) {
        send(clients[i]->sockfd, "READY?", 7, 0);
    }
    for (int i = 0; i < MAX_CLIENTS; i++) {
        flush_socket(clients[i]->sockfd);
    }

    int ready_flags[MAX_CLIENTS] = {0};
    while (ready_flags[0] == 0 || ready_flags[1] == 0) {
        for (int i = 0; i < MAX_CLIENTS; i++) {
            if (ready_flags[i]) continue;
            int n = recv(clients[i]->sockfd, buf, sizeof(buf) - 1, 0);
            if (n > 0) {
                buf[n] = '\0';
                if (strcasecmp(buf, "READY") == 0)
                    ready_flags[i] = 1;
            }
        }
    }
}

// 기억력 게임
int play_memory_game(client_info_t *c0, client_info_t *c1) {
    char seq[SEQ_LEN];
    for (int i = 0; i < SEQ_LEN; i++)
        seq[i] = 'A' + rand() % 26;
    char msg[128];
    snprintf(msg, sizeof(msg), "MEMORY %d", SEQ_LEN);
    for (int i = 0; i < SEQ_LEN; i++)
        snprintf(msg + strlen(msg), 16, " %c", seq[i]);
    send(c0->sockfd, msg, strlen(msg), 0);
    send(c1->sockfd, msg, strlen(msg), 0);
    char buf0[128], buf1[128];
    recv(c0->sockfd, buf0, sizeof(buf0), 0);
    recv(c1->sockfd, buf1, sizeof(buf1), 0);
    int ok0 = 1, ok1 = 1;
    char *tok;
    tok = strtok(buf0, " ");
    for (int i = 0; i < SEQ_LEN; i++) {
        tok = strtok(NULL, " "); if (!tok || tok[0] != seq[i]) ok0 = 0;
    }
    tok = strtok(buf1, " ");
    for (int i = 0; i < SEQ_LEN; i++) {
        tok = strtok(NULL, " "); if (!tok || tok[0] != seq[i]) ok1 = 0;
    }
    if (ok0 && !ok1) return c0->player_id;
    if (ok1 && !ok0) return c1->player_id;
    return rand() % MAX_CLIENTS;
}

// 연산 대결
int play_math_battle(client_info_t *c0, client_info_t *c1) {
    int a = rand() % 10 + 1, b = rand() % 10 + 1;
    char ops[] = "+-*/";
    char op = ops[rand() % 4];
    int res;
    switch (op) {
        case '+': res = a + b; break;
        case '-': res = a - b; break;
        case '*': res = a * b; break;
        case '/': res = b ? a / b : 0; break;
    }
    char msg[64];
    snprintf(msg, sizeof(msg), "MATH %d %c %d", a, op, b);
    send(c0->sockfd, msg, strlen(msg), 0);
    send(c1->sockfd, msg, strlen(msg), 0);
    char buf0[32], buf1[32];
    recv(c0->sockfd, buf0, sizeof(buf0), 0);
    recv(c1->sockfd, buf1, sizeof(buf1), 0);
    int ans0 = atoi(buf0), ans1 = atoi(buf1);
    int ok0 = (ans0 == res), ok1 = (ans1 == res);
    if (ok0 && !ok1) return c0->player_id;
    if (ok1 && !ok0) return c1->player_id;
    if (ok0 && ok1) return rand() % MAX_CLIENTS;
    return rand() % MAX_CLIENTS;
}

// 반응속도 배틀
int play_reaction_battle(client_info_t *c0, client_info_t *c1) {
    int delay = rand() % 3 + 1; sleep(delay);
    const char *msg = "REACT";
    send(c0->sockfd, msg, strlen(msg), 0);
    send(c1->sockfd, msg, strlen(msg), 0);
    fd_set rfds; struct timeval tv;
    FD_ZERO(&rfds);
    FD_SET(c0->sockfd, &rfds);
    FD_SET(c1->sockfd, &rfds);
    int maxfd = (c0->sockfd > c1->sockfd ? c0->sockfd : c1->sockfd) + 1;
    tv.tv_sec = 5; tv.tv_usec = 0;
    int ret = select(maxfd, &rfds, NULL, NULL, &tv);
    if (ret > 0) {
        if (FD_ISSET(c0->sockfd, &rfds)) {
            char buf[16]; recv(c0->sockfd, buf, sizeof(buf), 0);
            return c0->player_id;
        }
        if (FD_ISSET(c1->sockfd, &rfds)) {
            char buf[16]; recv(c1->sockfd, buf, sizeof(buf), 0);
            return c1->player_id;
        }
    }
    return rand() % MAX_CLIENTS;
}

void *handle_client(void *arg) {
    client_info_t *info = arg;
    char msg[64];
    snprintf(msg, sizeof(msg), "[서버] Player %d 접속!\n", info->player_id + 1);
    send(info->sockfd, msg, strlen(msg), 0);
    return NULL;
}

int main() {
    srand(time(NULL));

    int server_fd, client_fd;
    struct sockaddr_in addr;
    socklen_t sz = sizeof(addr);
    pthread_t tid;

    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd < 0) { perror("socket"); exit(EXIT_FAILURE); }

    int opt = 1;
    setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    addr.sin_family = AF_INET;
    addr.sin_port = htons(PORT);
    addr.sin_addr.s_addr = INADDR_ANY;
    memset(addr.sin_zero, 0, sizeof(addr.sin_zero));

    if (bind(server_fd, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
        perror("bind"); exit(EXIT_FAILURE);
    }

    if (listen(server_fd, MAX_CLIENTS) < 0) {
        perror("listen"); exit(EXIT_FAILURE);
    }

    printf("[서버] %d번 포트 대기중\n", PORT);

    int cnt = 0;
    while (cnt < MAX_CLIENTS) {
        client_fd = accept(server_fd, (struct sockaddr *)&addr, &sz);
        if (client_fd < 0) { perror("accept"); continue; }
        client_info_t *ci = malloc(sizeof(*ci));
        ci->sockfd = client_fd;
        ci->player_id = cnt;
        clients[cnt++] = ci;
        pthread_create(&tid, NULL, handle_client, ci);
        pthread_detach(tid);
    }

    int (*game_funcs[3])(client_info_t *, client_info_t *) = {
        play_memory_game, play_math_battle, play_reaction_battle
    };

    for (int round = 1; round <= 3; round++) {
        printf("[서버] Round %d 시작\n", round);
        int winner = game_funcs[round - 1](clients[0], clients[1]);

        pthread_mutex_lock(&game.lock);
        game.scores[winner]++;
        game.current_round++;
        pthread_mutex_unlock(&game.lock);

        for (int i = 0; i < MAX_CLIENTS; i++) {
            char result_msg[64];
            snprintf(result_msg, sizeof(result_msg), "ROUND%d WINNER: Player %d\n", round, winner + 1);
            send(clients[i]->sockfd, result_msg, strlen(result_msg), 0);
        }

        wait_for_ready();
    }

    int final_winner = (game.scores[0] > game.scores[1]) ? 0 :
                       (game.scores[1] > game.scores[0]) ? 1 : rand() % MAX_CLIENTS;

    printf("[서버] 최종 승자: Player %d\n", final_winner + 1);
    for (int i = 0; i < MAX_CLIENTS; i++) {
        char final_msg[64];
        snprintf(final_msg, sizeof(final_msg), "FINAL RESULT: %s\n",
                 (i == final_winner) ? "WINNER" : "LOSER");
        send(clients[i]->sockfd, final_msg, strlen(final_msg), 0);
        send(clients[i]->sockfd, "EXIT\n", 5, 0);
        close(clients[i]->sockfd);
        free(clients[i]);
    }
    close(server_fd);
    return 0;
}

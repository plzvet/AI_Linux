// multi_game_server.c
// 5개 미니게임 (기억력, 업다운, 연산 대결, 반응속도, 가위바위보) 5판 3선승 서버 코드

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <errno.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/time.h>
#include <signal.h>
#include <time.h>

#define PORT 10000
#define MAX_CLIENTS 2
#define MAX_SCORE 3
#define SEQ_LEN 6  // 기억력 게임 시퀀스 길이
#define PID_FILE "server.pid"

// 클라이언트 정보 구조
typedef struct {
    int sockfd;
    int player_id;
} client_info_t;

// 전체 게임 상태
typedef struct {
    int scores[MAX_CLIENTS];
    int current_round;
    pthread_mutex_t lock;
} game_state_t;

game_state_t game = {{0}, 0, PTHREAD_MUTEX_INITIALIZER};
client_info_t *clients[MAX_CLIENTS] = {NULL};

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

// 업다운 게임
int play_updown_game(client_info_t *c0, client_info_t *c1) {
    int target = rand() % 100 + 1;
    char msg[64];
    snprintf(msg, sizeof(msg), "UPDOWN 1 100");
    send(c0->sockfd, msg, strlen(msg), 0);
    send(c1->sockfd, msg, strlen(msg), 0);
    char buf0[32], buf1[32];
    recv(c0->sockfd, buf0, sizeof(buf0), 0);
    recv(c1->sockfd, buf1, sizeof(buf1), 0);
    int g0 = atoi(buf0), g1 = atoi(buf1);
    int d0 = abs(g0 - target), d1 = abs(g1 - target);
    if (d0 < d1) return c0->player_id;
    if (d1 < d0) return c1->player_id;
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

// 가위바위보 게임
int play_rps_game(client_info_t *c0, client_info_t *c1) {
    send(c0->sockfd, "RPS", 3, 0);
    send(c1->sockfd, "RPS", 3, 0);
    char b0[16], b1[16];
    recv(c0->sockfd, b0, sizeof(b0), 0);
    recv(c1->sockfd, b1, sizeof(b1), 0);
    int p0 = (b0[0] == 'R' ? 0 : b0[0] == 'P' ? 1 : 2);
    int p1 = (b1[0] == 'R' ? 0 : b1[0] == 'P' ? 1 : 2);
    if (p0 == p1) return rand() % MAX_CLIENTS;
    if ((p0 + 1) % 3 == p1) return c1->player_id;
    return c0->player_id;
}

// 클라이언트 처리 스레드
void *handle_client(void *arg) {
    client_info_t *info = arg;
    char msg[64];
    snprintf(msg, sizeof(msg), "[서버] Player %d 접속!", info->player_id + 1);
    send(info->sockfd, msg, strlen(msg), 0);
    while (1) {
        pthread_mutex_lock(&game.lock);
        if (clients[0] && clients[1]) {
            pthread_mutex_unlock(&game.lock);
            break;
        }
        pthread_mutex_unlock(&game.lock);
        usleep(100000);
    }
    return NULL;
}

void cleanup_pidfile() {
    remove(PID_FILE);
}

int main() {
    int server_fd, client_fd;
    struct sockaddr_in addr;
    socklen_t sz = sizeof(addr);
    pthread_t tid;
    FILE *pf;

    // 중복 실행 방지: 이전 인스턴스 종료
    pf = fopen(PID_FILE, "r");
    if (pf) {
        int oldpid;
        if (fscanf(pf, "%d", &oldpid) == 1 && kill(oldpid, 0) == 0) {
            printf("Existing server (pid %d) found. Killing...\n", oldpid);
            kill(oldpid, SIGTERM);
            sleep(1);
        }
        fclose(pf);
        remove(PID_FILE);
    }

    // 새로운 PID 기록
    pf = fopen(PID_FILE, "w");
    if (pf) {
        fprintf(pf, "%d\n", getpid());
        fclose(pf);
        atexit(cleanup_pidfile);
    }

    srand(time(NULL));
    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd < 0) { perror("socket"); exit(EXIT_FAILURE); }

    // SO_REUSEADDR 옵션 설정: TIME_WAIT 상태 포트 재사용 허용
    int opt = 1;
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }
    if (server_fd < 0) { perror("socket"); exit(EXIT_FAILURE); }

    addr.sin_family = AF_INET;
    addr.sin_port = htons(PORT);
    addr.sin_addr.s_addr = INADDR_ANY;
    memset(addr.sin_zero, 0, sizeof(addr.sin_zero));
    if (bind(server_fd, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
        if (errno == EADDRINUSE)
            fprintf(stderr, "Port %d already in use. Exiting.\n", PORT);
        else perror("bind");
        exit(EXIT_FAILURE);
    }

    if (listen(server_fd, MAX_CLIENTS) < 0) { perror("listen"); close(server_fd); exit(EXIT_FAILURE); }
    printf("[서버] %d번 포트 대기중\n", PORT);

    int cnt = 0;
    while (cnt < MAX_CLIENTS) {
        client_fd = accept(server_fd, (struct sockaddr *)&addr, &sz);
        if (client_fd < 0) { perror("accept"); continue; }
        client_info_t *ci = malloc(sizeof(*ci));
        if (!ci) { perror("malloc"); continue; }
        ci->sockfd = client_fd;
        ci->player_id = cnt;
        pthread_mutex_lock(&game.lock);
        clients[cnt++] = ci;
        pthread_mutex_unlock(&game.lock);
        pthread_create(&tid, NULL, handle_client, ci);
        pthread_detach(tid);
    }

    int (*game_funcs[5])(client_info_t *, client_info_t *) = {
        play_memory_game, play_updown_game,
        play_math_battle, play_reaction_battle,
        play_rps_game
    };

    while (game.current_round < 5) {
        printf("[서버] Round %d 시작\n", game.current_round + 1);
        int w = game_funcs[game.current_round](clients[0], clients[1]);
        pthread_mutex_lock(&game.lock);
        game.scores[w]++;
        game.current_round++;
        pthread_mutex_unlock(&game.lock);

        for (int i = 0; i < MAX_CLIENTS; i++) {
            send(clients[i]->sockfd, i == w ? "WIN" : "LOSE",
                 i == w ? 3 : 4, 0);
        }

        if (game.scores[w] >= MAX_SCORE) {
            printf("[서버] Player %d 승리\n", w + 1);
            for (int i = 0; i < MAX_CLIENTS; i++) {
                send(clients[i]->sockfd, "EXIT", 4, 0);
                close(clients[i]->sockfd);
                free(clients[i]);
            }
            close(server_fd);
            exit(EXIT_SUCCESS);
        }
    }

    return 0;
}

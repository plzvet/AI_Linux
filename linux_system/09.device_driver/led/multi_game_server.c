// multi_game_server.c
// 5개 미니게임 (기억력, 업다운, 연산 대결, 반응속도, 가위바위보) 5판 3선승 서버 코드

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <errno.h>
#include <fcntl.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/time.h>
#include <signal.h>
#include <time.h>

#define PORT 10000
#define MAX_CLIENTS 2
#define MAX_SCORE 3
#define SEQ_LEN 5  // 기억력 게임 시퀀스 길이
#define PID_FILE "server.pid"

// LED 제어를 위한 디바이스 드라이버 인터페이스
static void light_up_led(int winner_id) {
    int fd = open("/dev/led_control", O_RDWR);
    if (fd < 0) {
        perror("/dev/led_control");
        return;
    }
    // 1: Player1 승, 2: Player2 승
    char buf = (winner_id == 0) ? '1' : '2';
    if (write(fd, &buf, 1) < 0) {
        perror("LED write");
    }
    close(fd);
}

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

// (게임 함수 생략: 이전과 동일)
int play_memory_game(client_info_t *c0, client_info_t *c1) { /* ... */ }
int play_updown_game(client_info_t *c0, client_info_t *c1) { /* ... */ }
int play_math_battle(client_info_t *c0, client_info_t *c1) { /* ... */ }
int play_reaction_battle(client_info_t *c0, client_info_t *c1) { /* ... */ }
int play_rps_game(client_info_t *c0, client_info_t *c1) { /* ... */ }

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

    // 중복 실행 방지
    pf = fopen(PID_FILE, "r");
    if (pf) {
        int oldpid;
        if (fscanf(pf, "%d", &oldpid) == 1 && kill(oldpid, 0) == 0) {
            kill(oldpid, SIGTERM);
            sleep(1);
        }
        fclose(pf);
        remove(PID_FILE);
    }
    pf = fopen(PID_FILE, "w");
    if (pf) { fprintf(pf, "%d\n", getpid()); fclose(pf); atexit(cleanup_pidfile); }

    srand(time(NULL));
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
    if (listen(server_fd, MAX_CLIENTS) < 0) { perror("listen"); close(server_fd); exit(EXIT_FAILURE); }

    int cnt = 0;
    while (cnt < MAX_CLIENTS) {
        client_fd = accept(server_fd, (struct sockaddr *)&addr, &sz);
        if (client_fd < 0) { perror("accept"); continue; }
        client_info_t *ci = malloc(sizeof(*ci));
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
        int w = game_funcs[game.current_round](clients[0], clients[1]);
        pthread_mutex_lock(&game.lock);
        game.scores[w]++;
        game.current_round++;
        pthread_mutex_unlock(&game.lock);

        // 결과 통보 및 LED 제어
        for (int i = 0; i < MAX_CLIENTS; i++) {
            send(clients[i]->sockfd, i == w ? "WIN" : "LOSE", i == w ? 3 : 4, 0);
        }
        light_up_led(w);  // 승자에 따라 LED 점등

        if (game.scores[w] >= MAX_SCORE) {
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

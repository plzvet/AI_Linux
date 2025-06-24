#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <fcntl.h>
#include <sys/select.h>

#define PORT 10000
#define BUF_SIZE 128

typedef enum {
    MODE_IDLE, MODE_MEMORY, MODE_MATH, MODE_REACT, MODE_READY
} input_mode_t;

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <server_ip>\n", argv[0]);
        return 1;
    }
    int sockfd;
    struct sockaddr_in serv_addr;
    char buf[BUF_SIZE];
    input_mode_t mode = MODE_IDLE;

    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("socket"); exit(1);
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);
    if (inet_pton(AF_INET, argv[1], &serv_addr.sin_addr) <= 0) {
        perror("inet_pton"); exit(1);
    }

    if (connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        perror("connect"); exit(1);
    }
    printf("[클라이언트] 서버(%s:%d) 연결 성공\n", argv[1], PORT);

    while (1) {
        fd_set fds;
        FD_ZERO(&fds);
        FD_SET(sockfd, &fds);
        if (mode != MODE_IDLE)
            FD_SET(STDIN_FILENO, &fds);

        int maxfd = (sockfd > STDIN_FILENO ? sockfd : STDIN_FILENO) + 1;
        int ret = select(maxfd, &fds, NULL, NULL, NULL);
        if (ret < 0) { perror("select"); break; }

        // 서버 메시지 수신 처리
        if (FD_ISSET(sockfd, &fds)) {
            memset(buf, 0, BUF_SIZE);
            int n = recv(sockfd, buf, BUF_SIZE - 1, 0);
            if (n <= 0) {
                printf("[클라이언트] 서버 연결 종료\n");
                break;
            }
            buf[n] = '\0';

            if (strncmp(buf, "MEMORY", 6) == 0) {
                printf("[게임: 기억력] 시퀀스: %s\n", buf + 7);
                printf("정답을 재현하세요 (예: A B C D E): ");
                mode = MODE_MEMORY;
            }
            else if (strncmp(buf, "MATH", 4) == 0) {
                printf("[게임: 연산] 문제: %s\n정답 입력: ", buf + 5);
                mode = MODE_MATH;
            }
            else if (strncmp(buf, "REACT", 5) == 0) {
                printf("[게임: 반응속도] NOW! 엔터를 누르세요...\n");
                mode = MODE_REACT;
            }
            else if (strncmp(buf, "ROUND", 5) == 0 || strncmp(buf, "FINAL", 5) == 0) {
                printf("[결과] %s", buf);
            }
            else if (strncmp(buf, "READY?", 6) == 0) {
                printf("[알림] 다음 라운드를 시작하려면 'READY'를 입력하세요: ");
                mode = MODE_READY;
            }
            else if (strncmp(buf, "EXIT", 4) == 0) {
                printf("[게임 종료] 서버로부터 종료 명령 수신\n");
                break;
            }
            else {
                printf("%s", buf);
            }
        }

        // 사용자 입력 처리
        if (FD_ISSET(STDIN_FILENO, &fds)) {
            char input[BUF_SIZE];
            if (fgets(input, BUF_SIZE, stdin) != NULL) {
                input[strcspn(input, "\n")] = '\0';

                if (mode == MODE_READY) {
                    if (strcasecmp(input, "READY") == 0) {
                        send(sockfd, input, strlen(input), 0);
                        mode = MODE_IDLE;
                    } else {
                        printf("[경고] 'READY'만 입력 가능합니다. 다시 입력하세요: ");
                    }
                } else {
                    send(sockfd, input, strlen(input), 0);
                    mode = MODE_IDLE;
                }
            }
        }
    }

    close(sockfd);
    return 0;
}
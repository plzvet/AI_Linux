// multi_game_client.c
// 5개 미니게임 클라이언트 (기억력, 업다운, 연산 대결, 반응속도, 가위바위보)

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <time.h>

#define PORT 10000
#define BUF_SIZE 128

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <server_ip>\n", argv[0]);
        return 1;
    }
    int sockfd;
    struct sockaddr_in serv_addr;
    char buf[BUF_SIZE];

    // 소켓 생성
    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("socket"); exit(1);
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);
    if (inet_pton(AF_INET, argv[1], &serv_addr.sin_addr) <= 0) {
        perror("inet_pton"); exit(1);
    }

    // 서버 연결
    if (connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        perror("connect"); exit(1);
    }

    printf("[클라이언트] 서버(%s:%d) 연결 성공\n", argv[1], PORT);

    while (1) {
        // 서버 메시지 대기
        memset(buf, 0, BUF_SIZE);
        int n = recv(sockfd, buf, BUF_SIZE - 1, 0);
        if (n <= 0) {
            printf("[클라이언트] 서버 연결 종료\n");
            break;
        }
        buf[n] = '\0';

        // 메시지 타입 파싱
        if (strncmp(buf, "MEMORY", 6) == 0) {
            // 기억력 게임: "MEMORY <len> A B C ..."
            printf("[게임: 기억력] 시퀀스: %s\n", buf + 7);
            printf("정답을 재현하세요 (예: A B C D E): ");
            char response[BUF_SIZE];
            fgets(response, BUF_SIZE, stdin);
            response[strcspn(response, "\n")] = '\0';
            send(sockfd, response, strlen(response), 0);
        }
        else if (strncmp(buf, "UPDOWN", 6) == 0) {
            // 업다운: "UPDOWN min max"
            int min, max;
            sscanf(buf + 7, "%d %d", &min, &max);
            printf("[게임: 업다운] %d~%d 사이 숫자 추측: ", min, max);
            char input[BUF_SIZE];
            fgets(input, BUF_SIZE, stdin);
            input[strcspn(input, "\n")] = '\0';
            send(sockfd, input, strlen(input), 0);
        }
        else if (strncmp(buf, "MATH", 4) == 0) {
            // 연산 대결: "MATH a op b"
            printf("[게임: 연산] 문제: %s\n정답 입력: ", buf + 5);
            char input[BUF_SIZE];
            fgets(input, BUF_SIZE, stdin);
            input[strcspn(input, "\n")] = '\0';
            send(sockfd, input, strlen(input), 0);
        }
        else if (strncmp(buf, "REACT", 5) == 0) {
            // 반응속도 배틀: "REACT"
            printf("[게임: 반응속도] NOW! 엔터를 누르세요...\n");
            char dummy[BUF_SIZE];
            fgets(dummy, BUF_SIZE, stdin);
            send(sockfd, "HIT", 3, 0);
        }
        else if (strncmp(buf, "RPS", 3) == 0) {
            // 가위바위보: "RPS"
            printf("[게임: 가위바위보] R, P, S 중 하나 입력: ");
            char choice[BUF_SIZE];
            fgets(choice, BUF_SIZE, stdin);
            for (int i = 0; choice[i]; i++) {
                if (choice[i] == '\n') { choice[i] = '\0'; break; }
                if (choice[i] >= 'a' && choice[i] <= 'z') choice[i] -= 32;
            }
            send(sockfd, choice, strlen(choice), 0);
        }
        else if (strncmp(buf, "WIN", 3) == 0) {
            printf("[결과] 승리!\n");
        }
        else if (strncmp(buf, "LOSE", 4) == 0) {
            printf("[결과] 패배.\n");
        }
        else {
            // 초기 메시지 등 기타
            printf("%s\n", buf);
        }
    }

    close(sockfd);
    return 0;
}

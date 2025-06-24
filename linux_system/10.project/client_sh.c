// client.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/types.h>

#define PORT 10000
#define BUF_SIZE 256

int main(int argc, char *argv[]) {
    if(argc!=2){
        fprintf(stderr,"Usage: %s <server_ip>\n",argv[0]);
        return 1;
    }
    int sockfd=socket(AF_INET,SOCK_STREAM,0);
    struct sockaddr_in serv={.sin_family=AF_INET,.sin_port=htons(PORT)};
    inet_pton(AF_INET,argv[1],&serv.sin_addr);
    if(connect(sockfd,(struct sockaddr*)&serv,sizeof(serv))<0){
        perror("connect");exit(1);
    }
    printf("[클라이언트] 서버(%s:%d) 연결 성공\n",argv[1],PORT);

    // 소켓을 FILE*로 감싸서 fgets()로 한 줄씩 읽기
    FILE *fp = fdopen(sockfd,"r+");
    char buf[BUF_SIZE];
    int win=0,lose=0;

    while(fgets(buf,BUF_SIZE,fp)){
        // 결과
        if(strncmp(buf,"WIN\n",4)==0){ printf("[결과] 승리!\n"); win++; continue; }
        if(strncmp(buf,"LOSE\n",5)==0){ printf("[결과] 패배.\n"); lose++; continue; }

        // 기억력
        if(strncmp(buf,"MEMORY",6)==0){
            printf("[게임: 기억력] 시퀀스 : %s", buf+7);
            printf("정답을 입력하세요 (예: A B C D E): ");
            fflush(stdout);
            char resp[BUF_SIZE];
            if(!fgets(resp,BUF_SIZE,stdin)) break;
            resp[strcspn(resp,"\n")]=0;
            fprintf(fp,"%s\n",resp);
            fflush(fp);
            continue;
        }
        // 연산 대결
        if(strncmp(buf,"MATH",4)==0){
            printf("[게임: 연산] 문제 : %s", buf+5);
            printf("정답 입력: ");
            fflush(stdout);
            char in[BUF_SIZE];
            if(!fgets(in,BUF_SIZE,stdin)) break;
            in[strcspn(in,"\n")]=0;
            fprintf(fp,"%s\n",in);
            fflush(fp);
            continue;
        }
        // 반응속도
        if(strncmp(buf,"REACT\n",6)==0){
            printf("[게임: 반응속도] NOW! 엔터를 누르세요...\n");
            fflush(stdout);
            char d[BUF_SIZE];
            if(!fgets(d,BUF_SIZE,stdin)) break;
            fprintf(fp,"HIT\n");
            fflush(fp);
            continue;
        }
        // 최종 요약
        if(strncmp(buf,"[종료]",6)==0){
            printf("%s", buf);
            break;
        }
        // 기타 메시지
        fputs(buf, stdout);
    }

    fclose(fp);
    return 0;
}

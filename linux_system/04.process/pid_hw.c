#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>

// PPID 를 구하는 함수를 작성
// pid_t 는 PID 를 나타내는 자료형형
int get_ppid(pid_t pid) {  
    char path[256], line[256];
    // path: /proc/[pid]/status 경로 문자열을 저장할 배열
    // line: 파일에서 읽어온 한 줄의 내용을 저장할 배열
    
    snprintf(path, sizeof(path), "/proc/%d/status", pid);
    // 해당하는 경로를 문자열로 생성해서 path 에 입력
    // status 파일은 해당 프로세스의 상태 정보 포함 (이 중 PPid:가 필요)
    FILE *fp = fopen(path, "r");
    if (!fp) return -1; // 오류처리

    int ppid = -1; // ppid 를 저장할 변수 초기화 ( -1은 에러용 기본값 )
    while (fgets(line, sizeof(line), fp)) {
        if (strncmp(line, "PPid:", 5) == 0) {
            // PPid: 로 시작한다면,,
            sscanf(line, "PPid:\t%d", &ppid);
            // 숫자만 추출해서 ppid에 저장
            break;
        }
    }

    fclose(fp);
    return ppid; // 파싱된 ppid 를 반환
}

int main() {
    pid_t pid = getpid();  // 현재 프로세스 PID 를 얻음.
    printf("01.current : %d", pid); // 출력 시작: 현재 프로세스 ID 출력

    while (1) {
        int ppid = get_ppid(pid);
        if (ppid <= 0)
            break;
            // 부모 PID가 0 이하이면 루프 종료
            // 보통 PID 1(init)의 부모 PID는 0이므로, 루트까지 도달한 상태

        printf(" → %d", ppid);
        pid = ppid;  // 다음 루프에서 부모를 추적
    }

    printf("\n");
    return 0;
}

/* 
1. 현재 프로세스 ID에서 시작.

2. /proc/[pid]/status를 열어 PPid: 값을 찾음음

3. 최상위 프로세스(PID 1 또는 2)까지 부모 프로세스를 추적함.

4. 01.current : pid → ppid → pppid ... 형식으로 출력.
*/

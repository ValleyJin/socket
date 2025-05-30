#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define PORT "80"  // 문자열로 변경
#define HOST "example.com"
#define REQUEST "GET / HTTP/1.1\r\nHost: example.com\r\nConnection: close\r\n\r\n"

int main() {
    struct addrinfo hints, *res, *p;
    int sockfd;
    char buffer[4096];

    // 1. 도메인 → IP 주소 변환 (현대적 방식)
    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;  // IPv4/IPv6 모두 지원
    hints.ai_socktype = SOCK_STREAM;  // TCP 소켓

    int status = getaddrinfo(HOST, PORT, &hints, &res);
    if (status != 0) {
        fprintf(stderr, "getaddrinfo 오류: %s\n", gai_strerror(status));
        return 1;
    }

    // 2. 소켓 생성 및 연결
    for(p = res; p != NULL; p = p->ai_next) {
        sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol);
        if (sockfd == -1) continue;

        if (connect(sockfd, p->ai_addr, p->ai_addrlen) == 0) {
            break;  // 연결 성공
        }

        close(sockfd);
    }

    if (p == NULL) {
        fprintf(stderr, "연결 실패\n");
        freeaddrinfo(res);
        return 1;
    }

    // 3. HTTP 요청 전송 및 응답 수신
    // ... 기존 코드 유지 ...

    freeaddrinfo(res);  // 메모리 해제
    close(sockfd);
    return 0;
}
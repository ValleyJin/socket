#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/time.h>

#define PORT "80"  // 문자열로 변경
// 테스트용 공개 API 
#define HOST "httpbin.org"
#define REQUEST "GET /get HTTP/1.1\r\nHost: httpbin.org\r\n\r\n"

int main() {
    struct addrinfo hints, *res, *p;
    int sockfd, optval;
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

        // 2.1. 타임아웃 설정 (5초)
        struct timeval tv;
        tv.tv_sec = 5;
        tv.tv_usec = 0;
        if (setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv)) < 0 ||
            setsockopt(sockfd, SOL_SOCKET, SO_SNDTIMEO, &tv, sizeof(tv)) < 0) {
            perror("setsockopt 실패");
            close(sockfd);
            freeaddrinfo(res);
            return 1;
        }

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
int bytes_sent = send(sockfd, REQUEST, strlen(REQUEST), 0);
if (bytes_sent < 0) {
    perror("send 실패");
    close(sockfd);
    freeaddrinfo(res);
    return 1;
}

// 버퍼 초기화
int total_received = 0;

// 응답 수신 루프
while (1) {
    int bytes_received = recv(sockfd, buffer + total_received, 
                            sizeof(buffer) - total_received - 1, 0);
    if (bytes_received <= 0) break;
    total_received += bytes_received;
    if (total_received >= sizeof(buffer) - 1) break;
}

// 응답 출력
if (total_received > 0) {
    buffer[total_received] = '\0';
    printf("응답:\n%.*s\n", total_received, buffer);
}

    freeaddrinfo(res);  // 메모리 해제
    close(sockfd);
    return 0;
}
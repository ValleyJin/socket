# HTTP Raw Client 구현

## 레거시 코드 (`gethostbyname` 사용)의 문제점

### 1. **스레드 안전성 결여**
   - `gethostbyname()`은 내부 정적 버퍼 사용 → 멀티스레드 환경에서 데이터 경쟁 발생
   - **증상**: 동시 호출 시 크래시 또는 잘못된 결과 반환

### 2. **메모리 관리 문제**
   - 반환된 포인터가 정적 메모리를 가리켜 사용자 제어 불가
   - **예시**:
     ```c
     struct hostent *h1 = gethostbyname("example.com");
     struct hostent *h2 = gethostbyname("google.com"); 
     // h1이 가리키는 데이터도 google.com으로 변경됨
     ```

### 3. **프로토콜 한계**
   - IPv4 전용 (IPv6 미지원)
   - `gethostbyname()`은 RFC 2553에서 공식적으로 deprecated 됨

## 개선된 솔루션 (`getaddrinfo` 사용)

### 1. **스레드 안전성 보장**
   - 재진입 가능(Reentrant) 설계
   - 매 호출마다 새로운 메모리 할당

### 2. **향상된 메모리 관리**

   ```c
   struct addrinfo *res;
   getaddrinfo(..., &res);
   /* 사용 후 */
   freeaddrinfo(res);  // 명시적 해제
```
### 3. addrinfo 구조체의 주요 필드
addrinfo 구조체는 네트워크 주소 정보를 저장하는 현대적인 C 구조체입니다. 
getaddrinfo()와 함께 사용되어 네트워크 프로그래밍을 더 유연하고 안전하게 만들어줍니다. 특히 IPv4/IPv6 듀얼 스택 지원이 큰 장점입니다.

```c    
struct addrinfo {
    int              ai_flags;     // 추가 옵션 (AI_PASSIVE, AI_CANONNAME 등)
    int              ai_family;    // 주소 체계 (AF_INET, AF_INET6, AF_UNSPEC)
    int              ai_socktype;  // 소켓 타입 (SOCK_STREAM, SOCK_DGRAM)
    int              ai_protocol;  // 프로토콜 (IPPROTO_TCP, IPPROTO_UDP)
    socklen_t        ai_addrlen;   // ai_addr의 길이
    struct sockaddr *ai_addr;      // 소켓 주소 구조체 포인터
    char            *ai_canonname; // 정규 호스트 이름
    struct addrinfo *ai_next;      // 다음 항목을 가리키는 포인터 (링크드 리스트)
};
```

일반적인 사용패턴
```c
struct addrinfo hints = {
    .ai_family = AF_UNSPEC,    // IPv4/IPv6 모두 허용
    .ai_socktype = SOCK_STREAM, // TCP 소켓
    .ai_flags = AI_PASSIVE     // 서버 바인딩용
};

struct addrinfo *result;
getaddrinfo("example.com", "80", &hints, &result);
// 사용 후 반드시 해제
freeaddrinfo(result);
```

멀티 호스트 지원
- ai_next 필드로 연결된 링크드 리스트 구조
- 여러 IP 주소가 있을 경우 순회 가능:

```c
for (struct addrinfo *p = result; p != NULL; p = p->ai_next) {
    // 각 주소별 처리
}
```

## 컴파일 및 실행 방법

- o 옵션을 활용하여 실행파일의 이름을 지정
```bash
gcc -o http_client http_raw_client.c
./http_client
```

## 주의 사항
- 반드시 `freeaddrinfo()`로 메모리 해제 필요
- 연결 실패 시 `ai_next`로 다음 주소 자동 시도

## 향후 개선 방향
- HTTPS 지원 추가
- 타임아웃 설정 구현
- 더 강력한 에러 핸들링

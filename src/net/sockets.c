#include "../../include/net/sockets.h"
#include <pthread.h>
#ifdef TARGET_WIN
#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#else
#include <arpa/inet.h>
#include <netinet/in.h>
#include <poll.h>
#include <sys/socket.h>
#include <unistd.h>
#endif
#include <stdio.h>

int32_t sockets_open_server(const char *ip_address, uint32_t port) {
  int opt = 1;
#ifdef TARGET_WIN
  SOCKET sock = socket(AF_INET, SOCK_STREAM, 0);
  setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, (const char *)&opt, sizeof(opt));
#else
  int sock = socket(AF_INET, SOCK_STREAM, 0);
  setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
#endif
  struct sockaddr_in server_socket = {.sin_family = AF_INET, .sin_port = htons(port)};
  if (inet_pton(AF_INET, ip_address, &server_socket.sin_addr) <= 0) {
    perror("inet_pton failed (invalid IP)");
    sockets_close(sock);
    return -1;
  }

#ifdef TARGET_WIN
  if (sock == SOCKET_ERROR) {
    fprintf(stderr, "Socket failed: %d\n", WSAGetLastError());
    return -1;
  }
#else
  if (sock < 0) {
    perror("socket failed");
    return -1;
  }
#endif

  if (bind(sock, (struct sockaddr *)&server_socket, sizeof(server_socket)) < 0) {
    perror("bind failed");
    sockets_close(sock);
    return -1;
  }

  if (listen(sock, 10) < 0) {
    perror("listen failed");
    sockets_close(sock);
    return -1;
  }

  return sock;
}

int32_t sockets_connect_to_server(const char *ip_address, uint32_t port) {
#ifdef TARGET_WIN
  SOCKET sock = socket(AF_INET, SOCK_STREAM, 0);
#else
  int sock = socket(AF_INET, SOCK_STREAM, 0);
#endif
  if (sock < 0) {
    perror("socket failed");
    return -1;
  }

  struct sockaddr_in server = {.sin_family = AF_INET, .sin_port = htons(port)};

  if (inet_pton(AF_INET, ip_address, &server.sin_addr) <= 0) {
    perror("invalid address");
    sockets_close(sock);
    return -1;
  }

  if (connect(sock, (struct sockaddr *)&server, sizeof(server)) < 0) {
    perror("connect failed");
    sockets_close(sock);
    return -1;
  }
  return sock;
}

int64_t sockets_server_accept_client(int32_t socket_addr) {
  struct sockaddr_in client_addr;
  socklen_t client_len = sizeof(client_addr);
#ifdef TARGET_WIN
  SOCKET client_fd = accept(socket_addr, (struct sockaddr *)&client_addr, &client_len);
#else
  int client_fd = accept(socket_addr, (struct sockaddr *)&client_addr, &client_len);
#endif

  if (client_fd < 0) {
    perror("accept failed");
    sockets_close(socket_addr);
    return -1;
  }

  return client_fd;
}

int64_t sockets_send(int32_t socket_addr, SocketDataBuffer buf, int32_t flags) {
  return send(socket_addr, buf.buffer, buf.buffer_size, 0);
}

int64_t sockets_receieve(int32_t socket_addr, SocketDataBuffer buf, int32_t flags) {
  return recv(socket_addr, buf.buffer, buf.buffer_size, 0);
}

void sockets_server_listen_to_clients(ConnectionHandleFunc connection_handle_func) {}

int64_t sockets_server_poll_clients(PollClient *client_addresses, uint64_t client_addresses_amount, int32_t timeout) {
  if (client_addresses_amount > 0) {

#ifdef TARGET_WIN
    int poll_result = WSAPoll(client_addresses, client_addresses_amount, timeout);
#else
    int poll_result = poll(client_addresses, client_addresses_amount, timeout);
#endif
    return poll_result;
  }
  return 0;
}

void sockets_server_handle_poll(int64_t result) {
#ifdef TARGET_WIN
  if (result == SOCKET_ERROR) {
    fprintf(stderr, "WSAPoll failed: %d\n", WSAGetLastError());
  }
#else
  if (result < 0) {
    perror("poll failed");
  }
#endif
}

void sockets_close(int socket_addr) {
#ifdef TARGET_WIN
  closesocket(socket_addr);
#else
  close(socket_addr);
#endif
}

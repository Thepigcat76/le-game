#ifdef TARGET_WIN
#define Rectangle winapiIsSoOldAndGrossSoMangleIt_Rectangle
#define CloseWindow winapiIsSoOldAndGrossSoMangleIt_CloseWindow
#define ShowCursor winapiIsSoOldAndGrossSoMangleIt_ShowCursor
#define LoadImage winapiIsSoOldAndGrossSoMangleIt_LoadImage
#define DrawText winapiIsSoOldAndGrossSoMangleIt_DrawText
#define DrawTextEx winapiIsSoOldAndGrossSoMangleIt_DrawTextEx
#define PlaySound winapiIsSoOldAndGrossSoMangleIt_PlaySound

#include <winsock2.h>
#include <windows.h>
#include <ws2tcpip.h>

#undef Rectangle
#undef CloseWindow
#undef ShowCursor
#undef LoadImage
#undef DrawText
#undef DrawTextEx
#undef PlaySound
#else
#include <poll.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>
#endif
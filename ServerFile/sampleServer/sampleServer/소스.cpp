#include <iostream>
#include <WinSock2.h>
#include <list>
#pragma comment(lib, "ws2_32.lib")

using namespace std;


SOCKET lSock;

list<SOCKET> g_clilist;

void sendAll(char* buf,int len,SOCKET selfsock) {
    list<SOCKET>::iterator it;
    for (it = g_clilist.begin(); it != g_clilist.end(); it++) {
        if (*it != selfsock) {
            send(*it, buf, len, 0);
            //cout << "to : " << *it << "from : " << selfsock << endl;
        }
    }
}

DWORD WINAPI threadProc(LPVOID lParam) {
    char buf[128] = { 0 };
    int nRecv = 0;
    SOCKET hClient = (SOCKET)lParam;
    cout << "new client connected" << endl;

    while ((nRecv = recv(hClient, buf, sizeof(buf), 0)) > 0) {
        cout <<hClient<< " - receved : " << buf << endl;
        //send(hClient, buf, sizeof(buf), 0);
        //cout << buf << endl;
        sendAll(buf, sizeof(buf), hClient);
        memset(buf, 0, sizeof(buf));
    }

    g_clilist.remove(hClient);
    shutdown(hClient, SD_BOTH);
    closesocket(hClient);
    cout << "client has been disconnected" << endl;
    return 0;
}

int main()
{
    WSADATA wsa = { 0 };
    if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) {
        //winsock startup error
    }

    lSock = socket(AF_INET, SOCK_STREAM, 0);
    if (lSock == INVALID_SOCKET) {
        // socket creation error
    }

    SOCKADDR_IN svraddr = { 0 };
    svraddr.sin_family = AF_INET;
    svraddr.sin_port = htons(8888);
    svraddr.sin_addr.S_un.S_addr = htonl(INADDR_ANY);
    if (bind(lSock, (SOCKADDR*)&svraddr, sizeof(svraddr)) == SOCKET_ERROR) {
        // bind error
    }

    if (listen(lSock, SOMAXCONN) == SOCKET_ERROR) {
        // listen error
    }
    LPDWORD threadId = 0;


    // client socket
    SOCKADDR_IN cliaddr = { 0 };
    int addrLen = sizeof(cliaddr);
    SOCKET hClient = 0;
    HANDLE hThread;
    // buffer


    // 소켓 생성하여 클라이언트와 연결, 클라이언트 주소 정보 cliaddr에 저장
    while ((hClient = accept(lSock, (SOCKADDR*)&cliaddr, &addrLen)) != INVALID_SOCKET) {
        cout << "connected" << endl;
        g_clilist.push_back(hClient);
        hThread = CreateThread(NULL, 0, threadProc, (LPVOID)hClient, 0, threadId);
        CloseHandle(hThread);
    }

    closesocket(lSock);

    WSACleanup();

    return 0;
}

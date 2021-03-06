#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <poll.h>
#include <iostream>
#include <unistd.h>
#include <cstring>
#include <thread>

using namespace std;

#include "networkConfig.hpp"
#include "TCP.hpp"
#include "TCPListener.hpp"
#include "Listeners.hpp"
#include "MusicSend.hpp"

/*
 * this class extends TCPListener
 * waits for new connections of its type
 * creates an new TCP object that handles that connection
 * and then throws that on to a thread managed by TCPListener
 */

UpListener::UpListener(): TCPListener(PORTTCP_UPLOAD)
{

}

void UpListener::serveClients()
{
    /*
    ServeUP* newUp;
    while (true)
    {
        int sock = getConnection();
        newUp = new ServeUP(sock);
        makeThread(newUp);
    }
    */
}

DnListener::DnListener(): TCPListener(PORTTCP_DOWNLOAD)
{

}

void DnListener::serveClients()
{
    /*
    ServeDN* newUp;
    while (true)
    {
        int sock = getConnection();
        newUp = new ServeDN(sock);
        makeThread(newUp);
    }
    */
}


MusicListener::MusicListener(): TCPListener(PORTTCP_MUSIC)
{

}

void MusicListener::serveClients()
{
    MusicSend* musicNetHandler;
    while (true)
    {
        int sock = getConnection();
        musicNetHandler = new MusicSend(sock, 5);
        makeThread(musicNetHandler);
    }
}

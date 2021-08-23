#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <poll.h>
#include <iostream>
#include <unistd.h>
#include <cstring>
#include <thread>
#include <atomic>

using namespace std;
#include "networkConfig.hpp"
#include "TCP.hpp"


/**
 * for sending generalTCP messages
 * @param protocol the protocol that the struct has
 */
void TCP::sendPTLData(int protocol)
{
    _toSend.protocol = protocol;

    // send
    if (send(_theSock, (const void*)&_toSend, sizeof(struct generalTCP), 0) < 0)
    {
        perror("send wackiness");
    }
}


/**
 * for sending raw data messages
 * @param data the raw data that we are to send
 */
void TCP::sendRawData(char& data, int len)
{
    if (send(_theSock, (const void*)&data, len, 0) < 0)
    {
        perror("send wackiness");
    }
}


/**
 * sends infoTCP struct over the socket
 * @param protocol the protocol that the struct has
 */
void TCP::sendPTL(int protocol)
{
    _infoSend.protocol = protocol;

    // send
    if (send(_theSock, (const void*)&_infoSend, sizeof(struct infoTCP), 0) < 0)
    {
        perror("send wackiness");
    }
}


/**
 * blocks until poll recieves new data
 * based off the expectingType enum is what we do with the data
 * for infoTCP and generalTCP, waits till enough data has collected and casts the raw data to
 * the corresponding buffer
 * for raw data, moves data to the rawData buffer in blocks of size equal to _sizeOfRawBlocks
 */
int TCP::getFromPoll(bool waitForFill)
{
    int peek;
    int len;
    int packSizeInfo = sizeof(_infoRecieve);
    int packSizeGeneral = sizeof(_toRecieve);
    if (poll(&_pfd, 1, 1000) > 0)
    {
        if (_theSock < 0)
        {
            printf("Socket: %d\n", _theSock);
            exit(-1);
        }
        peek = recv(_theSock, nullptr, 0, MSG_PEEK | MSG_DONTWAIT);

        // they broke the connection
        if (peek == 0)
        {
            printf("They hung up\n");
            exit(1);

            return POLLHUNGUP;
        }

        // error
        if (peek < 0)
        {
            printf("error: %d\n", peek);
            printf("socket: %d\n", _theSock);
            perror("1msg error");
            exit(-1);
            return POLLBAD;
        }

        if (waitForFill)
        {
            switch (_expectingType)
            {
                case recievingInfoTCP:
                    if (peek < packSizeInfo)
                    {
                        _infoRecieve.protocol = -1;
                    }
                    else
                    {
                        len = recv(_theSock, &_infoRecieve, packSizeInfo, 0);

                        if (len != packSizeInfo)
                        {
                            printf("len %d vs _packSize %d\n", len, packSizeInfo);
                            perror("Oh no:\n");
                        }
                    }
                    break;
                case recievingGeneralTCP:
                    if (peek < packSizeGeneral)
                    {
                        _toRecieve.protocol = -1;
                    }
                    else
                    {
                        len = recv(_theSock, &_toRecieve, packSizeGeneral, 0);

                        if (len != packSizeGeneral)
                        {
                            printf("len %d vs _packSize %d\n", len, packSizeGeneral);
                            perror("Oh no:\n");
                        }
                    }
                    break;
                case recievingRaw:
                    if (peek < _sizeOfRawBlocks)
                    {
                        // wait
                    }
                    else
                    {
                        len = recv(_theSock, &_rawDataIn, TCP_BUF, 0);

                        if (len != TCP_BUF)
                        {
                            printf("len %d vs _packSize %d\n", len, packSizeGeneral);
                            perror("Oh no:\n");
                        }
                    }
                    break;
                default:
                    cout << "bad switch\n";
            }
        }
    }

    return POLLOK;
}




/**
 * the functions that expose the buffers to who ever is using this class
 * should be protected so that only subclasses can use it, but whatever
 */
struct generalTCP& TCP::getInBuf()
{
    return _toRecieve;
}


struct generalTCP& TCP::getOutBuf()
{
    return _toSend;
}


struct infoTCP& TCP::getInfoInBuf()
{
    return _infoRecieve;
}


struct infoTCP& TCP::getInfoOutBuf()
{
    return _infoSend;
}


char& TCP::getRawInBuf()
{
    return *_rawDataIn;
}


char& TCP::getRawOutBuf()
{
    return *_rawDataOut;
}

int TCP::getRawInBufSize()
{
    return _inSize;
}

int TCP::getRawOutBufSize()
{
    return _outSize;
}

/**
 * the constructor for the tcp obj
 * becausae we are the server, a listener socket will
 * accept requests and hand it off to us
 * @param sock the socket we are using
 */
TCP::TCP(int sock, int rawDataBlockSize) : _theSock(sock), _sizeOfRawBlocks(rawDataBlockSize), _outSize(0), _inSize(0), _expectingType(recievingInfoTCP)
{
    // make our in out packets
    _toSend = makeGeneralTCPPack(PING);
    _toRecieve = makeGeneralTCPPack(PONG);
    _infoSend = makeInfoTCPPack(PING);
    _infoRecieve = makeInfoTCPPack(PONG);

    // set poll var
    _pfd.fd = _theSock;
    _pfd.events = POLLIN | POLLHUP;
    _pfd.revents = 0;

}


/**
 * makes a generalTCP pack
 */
struct generalTCP TCP::makeGeneralTCPPack(int ptl)
{
    struct generalTCP pack;
    gethostname(pack.name, 10);
    pack.protocol = ptl;

    return pack;
}


/**
 * makes a infoTCP pack
 *
 */
struct infoTCP TCP::makeInfoTCPPack(int ptl)
{
    struct infoTCP pack;
    gethostname(pack.name, 10);
    pack.protocol = ptl;

    return pack;
}

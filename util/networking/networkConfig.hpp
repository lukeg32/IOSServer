#ifndef NETWORK_H
#define NETWORK_H
// all the constants will be put here
#include <AL/al.h>
#include <AL/alc.h>
#include <AL/alext.h>
#include <AL/alut.h>

#define TCP_BUF   100000
#define SONG_BUF   50000
#define SOCKET_BUF 50000

#define POLLOK 0
#define POLLHUNGUP 1
#define POLLBAD -1
#define POLLNOTFULL 2

#define PORT  12345
#define PORTTCP_UPLOAD  54321
#define PORTTCP_DOWNLOAD  54322
#define PORTTCP_MUSIC  54323

#define PONG 1
#define PING 2
#define STATE 3
#define SPEED 4
#define CONNECT 5
#define MOVE 6
#define DUMP 7
#define INFO 8
#define SENDINGFILE 9
#define NEXTLINE 10
#define ENDDOWNLOAD 11
#define STARTSTREAM 12
#define MORESONG 13
#define SONGHEADER 14
#define ENDSONG 15
#define SENDINGFILEHEADER 16

#define SONGPATH "songs"

// a struct for sending info only
struct infoTCP
{
    char name[10];
    int protocol;
    struct timeval time;
};

// a struct for sending info and data
struct generalTCP
{
    char name[10];
    int protocol;
    size_t dataSize;
    struct timeval time;
    char data[SOCKET_BUF];
};

// struct that holds all the music header information that is needed to play the song
// TODO figure out what is needed to play music on ios
struct musicHeader
{
    uint8_t channels;
    int32_t sampleRate;
    uint8_t bitsPerSample;
    ALsizei dataSize;
    ALenum format;
    char name[30];
};
#endif

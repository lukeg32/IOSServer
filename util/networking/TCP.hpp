#ifndef TCP_H
#define TCP_H
#include <poll.h>
#include <atomic>
#include "networkConfig.hpp"

enum recievingState
{
    recievingGeneralTCP,
    recievingInfoTCP,
    recievingRaw,
};

class TCP {
    public:
        //don't care about this rn
        //bool waitForKey();
        //bool validate();

        /**
         * for sending generalTCP messages
         */
        void sendPTLData(int protocol);


        /**
         * for sending raw data messages
         */
        void sendRawData(char& data, int len);


        /**
         * for sending infoTCP messages
         */
        void sendPTL(int protocol);


        /**
         * blocks until poll recieves new data
         * based off the expectingType enum is what we do with the data
         * for infoTCP and generalTCP, waits till enough data has collected and casts the raw data to
         * the corresponding buffer
         * for raw data, moves data to the rawData buffer in blocks of size equal to _sizeOfRawBlocks
         */
        int getFromPoll();

        // give other classes access to the buffers
        struct generalTCP& getInBuf();
        struct generalTCP& getOutBuf();
        struct infoTCP& getInfoInBuf();
        struct infoTCP& getInfoOutBuf();
        char& getRawInBuf();
        char& getRawOutBuf();
        int getRawInBufSize();
        int getRawOutBufSize();

        TCP(int sock, int rawDataBlockSize);

        /*
         * the main function that does something with these functions
         * is to be implemented by a subclass
         */
        virtual void run(std::atomic<bool>* isDead) = 0;

    private:
        struct generalTCP makeGeneralTCPPack(int ptl);
        struct infoTCP makeInfoTCPPack(int ptl);

        // the in out stuct data buffers
        struct generalTCP _toSend;
        struct generalTCP _toRecieve;

        // in out buffers for minimal data transfers
        struct infoTCP _infoSend;
        struct infoTCP _infoRecieve;

        // in raw data buffer
        char _rawDataIn[TCP_BUF];
        int _inSize;

        // out raw data buffer
        char _rawDataOut[TCP_BUF];
        int _outSize;

        // enum to determine what we are expecting
        recievingState _expectingType;

        // tells us how much raw data we let accumulate before moving to buffer
        int _sizeOfRawBlocks;

        // socket stuff
        struct pollfd _pfd;
        int _theSock;

};

void progressBarThread(long& top, int& bottom, int width);
void progressBarWithBufThread(long& top, int& bottom, int width, int& numBuffs);
void drawProgress(double percent, int width);
void drawProgressWithBufCount(double percent, int width, int numBuffs);
void drawProgressRaw(double percent, int width);


#endif

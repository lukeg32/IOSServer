#include <AL/al.h>
#include <AL/alc.h>
#include <AL/alext.h>
#include <AL/alut.h>
#include <thread>

#include <cstring>
#include <stdio.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <poll.h>
#include <iostream>
#include <fstream>
#include <filesystem>

#include <bit>

using namespace std;

#include "networkConfig.hpp"
#include "MusicSend.hpp"
#include "TCP.hpp"

MusicSend::MusicSend(int sock, int blockSize) : TCP(sock, blockSize)
{
    ioctl(STDOUT_FILENO, TIOCGWINSZ, &_w);
    _barWidth = _w.ws_col - 16;
    _songsPlayed = 0;
}

string getNextSong()
{
    for (const auto& entry : filesystem::directory_iterator(SONGPATH))
        cout << entry.path() << endl;

    string a = ("songs/Haluve_BOOST.wav");
    return a;
}

void MusicSend::run(atomic<bool>* isDead)
{
    //step 1: wait for start streaming msg send header
    //step 2: wait for an infoTCP packet to ask for more info, send when it arrives
    //step 3: when done with song wait for confirmation from client
    //step 4: close tcp, (later) send header for new song go to step 2

    //test
    char* msg = "This is a test!";
    printf("msg: %s, size: %d\n", msg, strlen(msg));
    sendRawData(msg, strlen(msg));

    expectRaw();
    printf("Waiting for msg.\n");
    int returnValue = 0;
    bool waiting = true;
    while (waiting)
    {
        // wait for it to be a full thing
        returnValue = getFromPoll();
        if (returnValue == POLLOK)
            waiting = false;

        //printf("return value: %d\n", returnValue);
    }

    cout << "The msg: ";
    for (int i = 0; i < getRawInBufSize(); i++)
    {
        cout << (getRawInBuf())[i];
    }
    cout << endl;

    char* msg2 = "I hope your phone explodes!";
    printf("msg: %s, size: %d\n", msg2, strlen(msg2));
    sendRawData(msg2, strlen(msg2));

    closeConnection();
    printf("\nExit\n");
    *isDead = true;
}

char* load_wav(const string& filename, uint8_t& channels, int32_t& sampleRate, uint8_t& bitsPerSample, ALsizei& size, ALenum& format)
{
    ifstream in(filename, ios::binary);
    if(!in.is_open())
    {
        cerr << "ERROR: Could not open \"" << filename << "\"" << endl;
        return nullptr;
    }
    if(!load_wav_file_header(in, channels, sampleRate, bitsPerSample, size))
    {
        cerr << "ERROR: Could not load wav header of \"" << filename << "\"" << endl;
        return nullptr;
    }

    char* data = new char[size];

    in.read(data, size);

    if(channels == 1 && bitsPerSample == 8)
        format = AL_FORMAT_MONO8;
    else if(channels == 1 && bitsPerSample == 16)
        format = AL_FORMAT_MONO16;
    else if(channels == 2 && bitsPerSample == 8)
        format = AL_FORMAT_STEREO8;
    else if(channels == 2 && bitsPerSample == 16)
        format = AL_FORMAT_STEREO16;
    else
    {
        std::cerr
            << "ERROR: unrecognised wave format: "
            << channels << " channels, "
            << bitsPerSample << " bps" << std::endl;
        exit(EXIT_FAILURE);
    }

    return data;
}



bool load_wav_file_header(ifstream& file, uint8_t& channels, int32_t& sampleRate, uint8_t& bitsPerSample, ALsizei& size)
{
    char buffer[4];
    if(!file.is_open())
        return false;

    // the RIFF
    if(!file.read(buffer, 4))
    {
        cerr << "ERROR: could not read RIFF" << endl;
        return false;
    }


    if(strncmp(buffer, "RIFF", 4) != 0)
    {
        cerr << "ERROR: file is not a valid WAVE file (header doesn't begin with RIFF)" << endl;
        return false;
    }

    // the size of the file
    if(!file.read(buffer, 4))
    {
        cerr << "ERROR: could not read size of file" << endl;
        return false;
    }

    // the WAVE
    if(!file.read(buffer, 4))
    {
        cerr << "ERROR: could not read WAVE" << endl;
        return false;
    }
    if(strncmp(buffer, "WAVE", 4) != 0)
    {
        cerr << "ERROR: file is not a valid WAVE file (header doesn't contain WAVE)" << endl;
        return false;
    }

    // "fmt/0"
    if(!file.read(buffer, 4))
    {
        cerr << "ERROR: could not read fmt/0" << endl;
        return false;
    }

    // this is always 16, the size of the fmt data chunk
    if(!file.read(buffer, 4))
    {
        cerr << "ERROR: could not read the 16" << endl;
        return false;
    }

    // PCM should be 1?
    if(!file.read(buffer, 2))
    {
        cerr << "ERROR: could not read PCM" << endl;
        return false;
    }

    // the number of channels
    if(!file.read(buffer, 2))
    {
        cerr << "ERROR: could not read number of channels" << endl;
        return false;
    }
    channels = convert_to_int(buffer, 2);

    // sample rate
    if(!file.read(buffer, 4))
    {
        cerr << "ERROR: could not read sample rate" << endl;
        return false;
    }
    sampleRate = convert_to_int(buffer, 4);

    // (sampleRate * bitsPerSample * channels) / 8
    if(!file.read(buffer, 4))
    {
        cerr << "ERROR: could not read (sampleRate * bitsPerSample * channels) / 8" << endl;
        return false;
    }

    // ?? dafaq
    if(!file.read(buffer, 2))
    {
        cerr << "ERROR: could not read dafaq" << endl;
        return false;
    }

    // bitsPerSample
    if(!file.read(buffer, 2))
    {
        cerr << "ERROR: could not read bits per sample" << endl;
        return false;
    }
    bitsPerSample = convert_to_int(buffer, 2);
    //cerr << bitsPerSample << endl;

    // data chunk header "data"
    if(!file.read(buffer, 4))
    {
        cerr << "ERROR: could not read data chunk header" << endl;
        return false;
    }

    //cerr << buffer << endl;
    //file.read(buffer, 
    if(strncmp(buffer, "data", 4) != 0)
    {
        cerr << "ERROR: file is not a valid WAVE file (doesn't have 'data' tag)" << endl;
        return false;
    }

    // size of data
    if(!file.read(buffer, 4))
    {
        cerr << "ERROR: could not read data size" << endl;
        return false;
    }
    size = convert_to_int(buffer, 4);

    /* cannot be at the end of file */
    if(file.eof())
    {
        cerr << "ERROR: reached EOF on the file" << endl;
        return false;
    }
    if(file.fail())
    {
        cerr << "ERROR: fail state set on the file" << endl;
        return false;
    }

    return true;
}


int32_t convert_to_int(char* buffer, size_t len)
{
    int32_t a = 0;
    if(endian::native == endian::little)
        memcpy(&a, buffer, len);
    else
        for(size_t i = 0; i < len; ++i)
            reinterpret_cast<char*>(&a)[3 - i] = buffer[i];
    return a;
}

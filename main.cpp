#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <vector>
#include <iostream>
#include <string>
#include <iostream>

#include <thread>

#include <netdb.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <sys/socket.h>
#include <stdbool.h>
#include <sys/types.h>
#include <ifaddrs.h>

#include "util/networking/networkingConfig.hpp"
#include "util/networking/TCP.hpp"

using namespace std;

bool test_nw = true;

int main()
{
    if (test_nw)
    {
        printf("asdf\n");
        return 0;
    }

    printf("asdffff\n");
    return 0;
}

#pragma once

const int MAX_CONFIG_SIZE = 1000;

struct config
{
    char mode[8];
    char company[64];
    char ip[20];
    char dns[20];
    bool dhcp;
    bool igmp;
};

struct demostream
{
    char ip[20];
    // char mac[20];
    int port;
};

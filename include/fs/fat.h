#pragma once
void init_fat();
struct fat16_bpb{
    unsigned char eb3c90[3];
    char oemstr[8];
    short bytespersector;
    char sectorspercluster;
    short reservedsectors;
    char numfats;
    short numrootdirent;
    short totalsects;
    char mdt;
    short sectsperfat;
    short sectspertrack;
    short headorsides;
    long numhidsects;
    long numsects2;
    
    char biosdrivenum;
    char ntflag;
    char sig;
    unsigned int serial;
    char labelstring[11];
    long long cannotbetrusted;
    char code[448];
    short sigaa55;
}__attribute__((packed));




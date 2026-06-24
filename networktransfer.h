#ifndef NETWORKTRANSFER_H
#define NETWORKTRANSFER_H

bool SendFile(const char* filename, int port);

bool ReceiveFile(
    const char* outputFile,
    const char* ip,
    int port
);

#endif // NETWORKTRANSFER_H

/*
 * bang.h
 * 
 * class declaration file for the ArduinoCLI project
 * https://github.com/ripred/ArduinoCLI
 * 
 */
#ifndef  BANG_H_INCL
#define  BANG_H_INCL

#include <Arduino.h>
#include <Stream.h>
#include <SoftwareSerial.h>

class Bang {
private:
    // pointers to the cmd I/O stream and the optional serial output stream
    Stream *dbgstrm {nullptr};
    Stream *cmdstrm {nullptr};

public:
    // constructors
    Bang();
    Bang(Stream &cmd_strm);
    Bang(Stream &cmd_strm, Stream &dbg_strm);

    String send_and_recv(char const cmd_id, char const *pcmd);

    String exec(char const *pcmd);
    String macro(char const *pcmd);
    String serial(char const *pcmd);
    String compile_and_upload(char const *pcmd);

    long write_file(char const *filename, char const * const lines[], int const num);

    void push_me_pull_you(Stream &str1, Stream &str2);

    void sync();

}; // class Bang

#endif // BANG_H_INCL

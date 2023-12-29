/*
 * bang.cpp
 * 
 * class implementation file for the ArduinoCLI project
 * https://github.com/ripred/ArduinoCLI
 * 
 */
#include "Bang.h"

Bang::Bang() {
    dbgstrm = nullptr;
    cmdstrm = nullptr;
}

Bang::Bang(Stream &cmd_strm) :
    dbgstrm{nullptr},
    cmdstrm{&cmd_strm}
{
}

Bang::Bang(Stream &cmd_strm, Stream &dbg_strm) {
    dbgstrm = &dbg_strm;
    cmdstrm = &cmd_strm;
}

String Bang::send_and_recv(char const cmd_id, char const *pcmd) {
    if (!cmdstrm) { return ""; }

    String output = "";
    String cmd(String(cmd_id) + pcmd);
    Stream &stream = *cmdstrm;
    stream.println(cmd);
    delay(100);
    while (stream.available()) {
        output += stream.readString();
    }

    return output;
}

String Bang::exec(char const *pcmd) {
    return send_and_recv('!', pcmd);
}

String Bang::macro(char const *pcmd) {
    return send_and_recv('@', pcmd);
}

String Bang::compile_and_upload(char const *pcmd) {
    return send_and_recv('&', pcmd);
}

long Bang::write_file(char const *filename, char const * const lines[], int const num) {
    if (num <= 0) { return 0; }
    long len = 0;

    String cmd = String("echo \"") + lines[0] + "\" > " + filename;
    len += cmd.length();
    exec(cmd.c_str());

    for (int i=1; i < num; i++) {
        cmd = String("echo \"") + lines[i] + "\" >> " + filename;
        len += cmd.length();
        exec(cmd.c_str());
    }

    return len;
}

void Bang::push_me_pull_you(Stream &str1, Stream &str2) {
    if (str1.available() >= 2) {
        uint32_t const period = 20;
        uint32_t start = millis();
        while (millis() - start < period) {
            while (str1.available()) {
                str2.println(str1.readString());
            }
        }
    }
}

void Bang::sync() {
    if (!cmdstrm || !dbgstrm) { return; }
    push_me_pull_you(*cmdstrm, *dbgstrm);
    push_me_pull_you(*dbgstrm, *cmdstrm);
}

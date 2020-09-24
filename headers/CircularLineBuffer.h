#ifndef CIRCULAR_BUFFER_H
#define CIRCULAR_BUFFER_H

#include <cstdlib>
#include <mutex>
#include <string>
#ifndef _WIN32
    #include <unistd.h>
#endif

class CircularLineBuffer
{
    // Variables
    private:
    int _start, _count;
    size_t _buffer_size;
    char *_buffer;
    std::mutex _buffer_mtx;

    // Functions
    private:
    int _write(const char *, int);
    std::string _read();
    int _find_newline();
    int _next_free_index();
    int _n_free_spaces();
    int _shifted_index(int);
    bool _has_newline();
    bool _is_full();
    bool _is_empty();

    public:
    CircularLineBuffer();
    ~CircularLineBuffer();
    int write(const char *buffer, int buffer_length);
    std::string read();
};

#endif

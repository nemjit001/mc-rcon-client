#include "CircularLineBuffer.h"

CircularLineBuffer::CircularLineBuffer()
{
    this->_buffer_size = 4096;
    this->_buffer = (char *)calloc(this->_buffer_size, sizeof(char));
    this->_start = this->_count = 0;
}

CircularLineBuffer::~CircularLineBuffer()
{
    free(this->_buffer);
}

int CircularLineBuffer::_write(const char *buffer, int buffer_size)
{
    if (buffer_size > this->_n_free_spaces()) return -1;

    for (int i = 0; i < buffer_size; i++)
    {
        this->_buffer[this->_shifted_index(i)] = buffer[i];
        this->_count++;
    }

    return buffer_size;
}

std::string CircularLineBuffer::_read()
{
    std::string res;

    if (this->_is_empty() || !this->_has_newline()) return "";

    int end_idx = this->_find_newline();

    for (int i = 0; i < end_idx + 1; i++)
    {
        res += this->_buffer[this->_shifted_index(i)];
        this->_buffer[this->_shifted_index(i)] = '\0';

        this->_count--;
    }

    this->_start = end_idx;

    return res;
}

int CircularLineBuffer::_find_newline()
{
    for (int i = 0; i < (int)this->_buffer_size; i++)
    {
        if (this->_buffer[this->_shifted_index(i)] == '\n') return i;
    }
    return -1;
}

int CircularLineBuffer::_next_free_index()
{
    for (int i = 0; i < (int)this->_buffer_size; i++)
    {
        if (this->_buffer[this->_shifted_index(i)] == '\0') return i;
    }
    return -1;
}

int CircularLineBuffer::_n_free_spaces()
{
    return this->_buffer_size - this->_count;
}

int CircularLineBuffer::_shifted_index(int i)
{
    return (i + this->_start) % this->_buffer_size;
}

bool CircularLineBuffer::_has_newline()
{
    return this->_find_newline() != -1;
}

bool CircularLineBuffer::_is_full()
{
    return this->_count == (int)this->_buffer_size;
}

bool CircularLineBuffer::_is_empty()
{
    return this->_count == 0;
}

int CircularLineBuffer::write(const char *buffer, int buffer_size)
{
    this->_buffer_mtx.lock();
    auto res = this->_write(buffer, buffer_size);
    this->_buffer_mtx.unlock();
    
    return res;
}

std::string CircularLineBuffer::read()
{
    this->_buffer_mtx.lock();
    auto res = this->_read();
    this->_buffer_mtx.unlock();

    return res;
}

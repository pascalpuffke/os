#pragma once

#include <kernel/heap/memorymanager.h>

// Just a simple string class.
// Wraps around a char array, grows as needed.
class String final {
public:
    String() = default;
    String(const char*) = default;
    String(const String&) = default;
    String(String&&) = default;
    ~String() = default;

    String& operator=(const String&) = default;
    String& operator=(String&&) = default;

    String& operator+=(const String& other)
    {
    }

    String& append(const String& other)
    {
    }

    void clear()
    {
    }

    void reserve(usize size)
    {
    }

private:
    char* m_data;
    usize m_size;
    usize m_capacity;
};

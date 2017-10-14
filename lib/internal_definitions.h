#ifndef INTERNAL_DEFINITIONS_H
#define INTERNAL_DEFINITIONS_H

const size_t MB = 1024*1024;

enum class EntryTypes : uint8_t {
    RegularFile = 0,
    Directory = 1,
    Link = 3
};

#endif // INTERNAL_DEFINITIONS_H

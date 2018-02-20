#ifndef INTERNAL_DEFINITIONS_H
#define INTERNAL_DEFINITIONS_H

#include <string>

const size_t MB = 1024*1024;

enum class EntryTypes : uint8_t {
    RegularFile = 0,
    Directory = 1,
    Link = 3
};

const std::string TableOfContents::DEFAULT_OWNER = "__PARTS_DEFAULT_OWNER__";
const std::string TableOfContents::DEFAULT_GROUP = "__PARTS_DEFAULT_GROUP__";

#endif // INTERNAL_DEFINITIONS_H

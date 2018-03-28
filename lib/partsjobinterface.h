#pragma once

#include "baseentry.h"

namespace parts {

class PartsJobInterface {
public:
    PartsJobInterface() = default;
    virtual ~PartsJobInterface() = default;

    PartsJobInterface(const PartsJobInterface&) = delete;
    PartsJobInterface& operator=(const PartsJobInterface&) = delete;

    PartsJobInterface(PartsJobInterface&&) = delete;
    PartsJobInterface& operator=(PartsJobInterface&&) = delete;

    virtual operator bool() const = 0;
    virtual const BaseEntry* nextEntry() const = 0;
    /** checkExisting is to continue an aborted job */
    virtual std::string doNext(bool checkExisting) = 0;
};

}


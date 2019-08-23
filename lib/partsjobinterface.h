#pragma once

#include "baseentry.h"

namespace parts {

class PartsJobInterface {
public:
    PartsJobInterface() noexcept = default;
    virtual ~PartsJobInterface() noexcept = default;

    PartsJobInterface(const PartsJobInterface&) = delete;
    PartsJobInterface& operator=(const PartsJobInterface&) = delete;

    PartsJobInterface(PartsJobInterface&&) = delete;
    PartsJobInterface& operator=(PartsJobInterface&&) = delete;

    virtual operator bool() const = 0;
    virtual const BaseEntry* nextEntry() const = 0;
    virtual void doNext() = 0;
    /** Doesn't process the job, only moves to the next */
    virtual void skip() = 0;
};

}


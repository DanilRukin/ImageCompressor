#pragma once
#include <ctime>

class Stopwatch
{
    clock_t tStart;
    clock_t tCreate;
    double tValue;
    bool _enabled;

public:
    ~Stopwatch()
    {

    }

    Stopwatch() :
        tCreate(clock()),
        tStart(clock()),
        tValue(0.),
        _enabled(false)
    {

    }

    bool Start() {
        if (_enabled) return false;
        tStart = clock();
        _enabled = true;
        return true;
    }

    bool Stop() {
        if (!_enabled) return false;
        tValue += double(clock() - tStart) / CLOCKS_PER_SEC;
        _enabled = false;
        return true;
    }

    double Get() {
        return tValue + (_enabled ? double(clock() - tStart) / CLOCKS_PER_SEC : 0.);
    }

    void Reset() {
        tValue = 0.;
        tStart = clock();
    }
};

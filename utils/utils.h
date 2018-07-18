//
// Created by root on 7/17/18.
//

#ifndef ARM_BLDC_UTILS_H
#define ARM_BLDC_UTILS_H

#include <vector>

using std::vector;

class utils
{
public:
    static vector<int> range(const int first, const int last, const int step);
    static vector<int> range(const int last);
};


#endif //ARM_BLDC_UTILS_H

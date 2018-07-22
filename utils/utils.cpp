//
// Created by root on 7/17/18.
//

#include "utils.h"
#include "mbed.h"

#include <algorithm>
using std::for_each;


vector<int> utils::range(const int first, const int last, const int step)
{

    int size = ((last - first)/step);
    int counter = first;

    if(size < 0)
        size = 0;
    vector<int> sequence(size);
    for_each(begin(sequence), end(sequence), [&](int &n){ n = counter; counter += step;});

    return sequence;
}


// -----------------------------------------------------------------------------
/*!  \brief create a vector of integers from 0 to (last - 1)  at interval 1

      \param last  outer bound of sequence is not in sequence

      \return result    vector
 */
vector<int> utils::range(const int last)
{

    int step = 1;
    int first = 0;
    int size = ((last - first)/step);
    int counter = first;

    if(size < 0)
        size = 0;
    vector<int> sequence(size);
    for_each(begin(sequence), end(sequence), [&](int &n){ n = counter; counter += step;});

    return sequence;
}


volatile unsigned long long  _millis;

void millisStart(void)
{
    //SysTick_Config(SystemCoreClock / 1000);
}

/*extern "C" void SysTick_Handler(void)
{
    _millis++;
}*/

unsigned long long millis(void)
{
    return _millis;
}

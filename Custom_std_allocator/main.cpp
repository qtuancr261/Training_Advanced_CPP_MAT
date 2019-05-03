#include "poolallocator.h"
#include "minimalpoolalloc.h"
#include <iostream>
#include <set>
using std::cout;
using std::endl;
using std::set;
using EventSentHolder = set<long, std::less<long>, pool_allocator<long>>;
using EventSentHolder_mini = set<long , std::less<long>, pool_tallocator<long>>;
int main()
{
    EventSentHolder _eventSent;
    _eventSent.insert(10);
    _eventSent.insert(20);
    cout << "size of char : " << sizeof(char) << endl;
    return 0;
}
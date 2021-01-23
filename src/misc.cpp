#ifndef MISC_CPP
#define MISC_CPP

#include <vector>

void remove_element_by_value(std::vector<int> &cols, int val) {
    std::vector<int>::iterator it = cols.begin();
    while(it != cols.end()) {
        if(*it == val) {
            cols.erase(it);
            break;
        } 
        it++;
    }
}

#endif
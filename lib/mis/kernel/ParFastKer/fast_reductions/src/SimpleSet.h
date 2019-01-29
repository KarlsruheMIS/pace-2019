#ifndef SIMPLE_SET_H
#define SIMPLE_SET_H

#include <vector>

class SimpleSet
{
public:
    SimpleSet(size_t const size, bool const init) : elements(size, init)
    {
    }

    SimpleSet() : elements()
    {
    }

    ~SimpleSet() {}

    void Resize(size_t const size)
    {
        elements.resize(size, false);
    }

    bool Contains(int const x) const {
        assert(x < elements.size());
        return elements[x];
    }

    void Insert(int const x) {
        assert(x < elements.size());
        elements[x] = true;
    }

    void Remove(int const x) {
        assert(x < elements.size());
        elements[x] = false;
    }

    size_t Size()  const {
        int count = 0;
        for(auto element: elements) {
            if (element) {
                count++;
            }
        }
        return count; 
    }

private:
    std::vector<char> elements;
};

#endif // SIMPLE_SET_H

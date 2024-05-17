# Deque
This is a C++ implementation of a double-ended queue (deque) data structure using fixed-size arrays for storage. A deque is a data structure that supports insertion and deletion of elements from both ends. This implementation is templated, allowing it to handle elements of any simple data type.

## Functions

- `push_back`: Adds an element to the back of the deque
- `push_front`: Adds an element to the front of the deque
- `pop_back`: Removes an element from the back of the deque
- `pop_front`: Removes an element from the front of the deque
- `front`: Returns a reference to the first element in the deque
- `back`: Returns a reference to the last element in the deque
- `operator[]`: Provides access to elements at specific indices in the deque
- `operator=`: Makes a deep copy of a deque
- `empty`: Checks if the deque is empty
- `clear`: Removes all elements from the deque
- `size`: Returns the number of elements in the deque
- `insert`: Inserts an element at the specified index
- `remove`: Deletes the element from the specified index

  
## Features

- **Iterator Support**: Provides iterators for traversal and manipulation of elements.
- **Templated**: This deque class is templated.
- **Fixed-sized Arrays**: The arrays are of size 8

### Example Usage

```cpp
#include"Deque.h";

int main()
{
    // Create a deque of integers
    Deque<int> myDeque;

    // Insert elements
    myDeque.push_back(10);
    myDeque.push_front(20);

    // Access elements
    std::cout << "Front element: " << myDeque.front() << std::endl;
    std::cout << "Back element: " << myDeque.back() << std::endl;
    std::cout << "The 2nd element " << myDeque[1] << std::endl;

    // Iterate over elements
    std::cout << "Deque elements:";
    for (auto it = myDeque.begin(); it != myDeque.end(); ++it) {
        std::cout << " " << *it;
    }

    return 0;
}

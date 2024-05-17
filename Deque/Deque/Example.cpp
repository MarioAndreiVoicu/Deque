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

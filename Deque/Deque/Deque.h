#pragma once
#include<list>
#include<array>
#include<iostream>

template<typename T>
class Deque
{
	static const int mBlockSize = 8;
	static const int mLastValidIndex = 7;
	static const int mInitialIndex = 0;

	struct Block
	{
		std::array<T, mBlockSize> data;
		Block* previous;
		Block* next;

		Block() : previous(nullptr), next(nullptr) {}
	};

	std::list<Block*> blockAddresses;
	Block* firstBlock;
	Block* lastBlock;
	int firstBlockIndex, lastBlockIndex;

public:
	Deque();

	Deque(const Deque& other);

	class Iterator
	{
		friend class Deque;

		Deque& dequeReference;

		Block* block;
		int blockIndex;

	public:
		Iterator() : block(nullptr), blockIndex(mInitialIndex), dequeReference(nullptr) {}
		Iterator(Deque& dequeRef) : dequeReference(dequeRef), block(nullptr), blockIndex(mInitialIndex) {}

		T* operator->();
		T& operator*();
		Iterator& operator=(const Iterator& other);
		bool operator==(const Iterator& other);
		bool operator!=(const Iterator& other);
		Iterator& operator++();
		Iterator& operator--();
		Iterator operator++(int);
		Iterator operator--(int);
		Iterator operator+(int stepsForward);
		Iterator operator-(int stepsBackwards);
		Iterator& operator+=(int steps);
		Iterator& operator-=(int steps);
	};

	void push_back(const T& value);

	void push_front(const T& value);

	void pop_back();

	void pop_front();

	T& front();

	T& back();

	T& operator[](int index);

	Deque& operator=(const Deque& other);

	bool empty();

	void clear();

	int size();

	void insert(const T& value, int index);

	void remove(int index);

	Iterator begin();

	Iterator end();
};

template<typename T>
Deque<T>& Deque<T>::operator=(const Deque& other)
{
	if (this == &other)  //prevent self-assignment
		return *this;

	clear();

	for (const Block* block : other.blockAddresses)
	{
		Block* newBlock = new Block;
		newBlock->data = block->data;
		if (blockAddresses.empty() && firstBlockIndex != mBlockSize)
			blockAddresses.push_front(newBlock);
		else
			blockAddresses.push_back(newBlock);
	}

	//make the prev-next connections
	auto prevIt = blockAddresses.begin();
	for (auto it = std::next(prevIt); it != blockAddresses.end(); ++it, ++prevIt)
	{
		(*prevIt)->next = (*it);
		(*it)->previous = (*prevIt);
	}

	if (other.firstBlock != nullptr)
		firstBlock = blockAddresses.front();
	if (other.lastBlock != nullptr)
		lastBlock = blockAddresses.back();

	firstBlockIndex = other.firstBlockIndex;
	lastBlockIndex = other.lastBlockIndex;

	return *this;
}

template<typename T>
Deque<T>::Deque() : firstBlock(nullptr), lastBlock(nullptr), firstBlockIndex(mBlockSize), lastBlockIndex(mInitialIndex - 1) {}

template<typename T>
Deque<T>::Deque(const Deque& other)
{
	*this = other;
}

template<typename T>
void Deque<T>::push_back(const T& value)
{
	if (lastBlockIndex == mLastValidIndex)  //for the case of push_back in a full array,a new block is created
	{
		Block* newBlock = new Block;  //make a new block
		newBlock->previous = lastBlock;  //update the conection between the new block and the block that was previously last
		lastBlock->next = newBlock;
		newBlock->next = nullptr;
		lastBlock = newBlock;  //update the pointer to the last block
		lastBlockIndex = mInitialIndex - 1;  //update the pointer to the last element of the last block
		blockAddresses.push_back(newBlock);

		if (firstBlock == nullptr)  //if there is no firstBlock but there is a full block and a last block,then the full block becomes the firstBlock
		{
			firstBlock = blockAddresses.front();
			Block* tempAddress = blockAddresses.front();  //this is used to make the connections between the new firstBlock and the block after it
			tempAddress = tempAddress->next;  //now tempAddress has the address of the next block after the firstBlock
			Block*& nextBlockAfterFirstBlock = tempAddress;  //used a reference so that i can update the block after the firstBlock's previous
			firstBlock->next = nextBlockAfterFirstBlock;
			nextBlockAfterFirstBlock->previous = firstBlock;
			firstBlockIndex = mInitialIndex;  //the block is full so the index points at the last element,which in the firstBlock is index 0
		}
	}
	else if (lastBlock == nullptr && firstBlock != nullptr)  //if there is a first block but there is no last block,the lastBlock is created and the element in inserted in it
	{
		Block* newLastBlock = new Block;  //make a new block
		newLastBlock->previous = blockAddresses.back();  //update the conection between the new block and the block that was previously last
		Block*& lastFirstBlock = blockAddresses.back();  //this is is the block from the front that is closest to the middle
		lastFirstBlock->next = newLastBlock;
		lastBlock = newLastBlock;  //update the pointer to the last block
		lastBlockIndex = mInitialIndex - 1;  //update the pointer to the last element of the last block
		blockAddresses.push_back(newLastBlock);
	}
	else if (lastBlock == nullptr && firstBlock == nullptr)  //if the dequeue does not have a lastBlock or a firstBlock,the lastBlock is created
	{
		Block* newLastBlock = new Block;  //make a new lastBlock
		//newLastBlock->previous = newFirstBlock;  //update the conection between the firstBlock and lastBlock
		newLastBlock->next = nullptr;
		lastBlockIndex = mInitialIndex - 1;  //update the pointer to the last element of the last block
		lastBlock = newLastBlock;  //update the pointer to the last block
		blockAddresses.push_back(newLastBlock);
	}
	lastBlockIndex++;  //the lastBlockIndex points to the last element of the deque.At first it it initialized with -1 because there are no elements in the deque for it to point to.This way i increment it first and add the value in the deque at the incremented value
	lastBlock->data[lastBlockIndex] = value;
}

template<typename T>
void Deque<T>::push_front(const T& value)
{
	if (firstBlockIndex == mInitialIndex)		//for the case of push_front in a full block,a new block is made
	{
		Block* newBlock = new Block;  //make a new block
		firstBlock->previous = newBlock;  //update the conection between the new block and the previous first block
		newBlock->previous = nullptr;
		newBlock->next = firstBlock;
		firstBlock = newBlock;  //update the pointer to the first block
		firstBlockIndex = mBlockSize;  //update the pointer to the last element of the first block
		blockAddresses.push_front(newBlock);

		if (lastBlock == nullptr)  //if there is no lastBlock but there is a full block and a first block,then the full block becomes the lastBlock
		{
			lastBlock = blockAddresses.back();
			Block* tempAddress = blockAddresses.back();  //this is used to make the connections between the new lastBlock and the block before it
			tempAddress = tempAddress->previous;  //now tempAddress has the address of the previous block before the lastBlock
			Block*& previousBlockBeforeLastBlock = tempAddress;  //used a reference so that i can update the block after the firstBlock's previous
			lastBlock->previous = previousBlockBeforeLastBlock;
			previousBlockBeforeLastBlock->next = lastBlock;
			lastBlockIndex = mLastValidIndex;
		}
	}
	else if (firstBlock == nullptr && lastBlock != nullptr)  //if there is a last block but there is no first block,the firstBlock is created
	{
		Block* newFirstBlock = new Block;  //make a new block
		newFirstBlock->next = blockAddresses.front();  //update the conection between the new block and the block that was from the last blocks the closest to the middle
		Block*& firstLastBlock = blockAddresses.front();  //this is is the block from the end that is closest to the middle
		firstLastBlock->previous = newFirstBlock;
		firstBlock = newFirstBlock;  //update the pointer to the first block
		firstBlockIndex = mBlockSize;  //update the pointer to the last element of the last block
		blockAddresses.push_front(newFirstBlock);
	}
	else if (firstBlock == nullptr) //if the dequeue is does not have a firstBlock or a lastBlock,a firstBlock is created
	{
		Block* newFirstBlock = new Block;  //make a new firstBlock
		/*newFirstBlock->next = newLastBlock;*/
		newFirstBlock->previous = nullptr;
		firstBlockIndex = mBlockSize;  //update the pointer to the last element of the first block
		firstBlock = newFirstBlock;  //update the pointer to the first block
		blockAddresses.push_front(newFirstBlock);
	}
	firstBlockIndex--;
	firstBlock->data[firstBlockIndex] = value;
}

template<typename T>
void Deque<T>::pop_back()
{
	if (empty())
	{
		return;
	}
	else if (firstBlock != nullptr && lastBlock == nullptr)  //if there is only one block and it is the front block  (the condition is correct to check if there is only the firstBlock in the deque because if there are 2 blocks created with push_front() then the one that is filled becomes lastBlock so if there is a firstBlock but no lastBlock,then there is only 1 block and that is the firstBlock)
	{
		if (firstBlockIndex == mLastValidIndex)  //if there is only one element in the firstBlock and it is being deleted,the deque becomes empty
		{
			firstBlock->previous = nullptr;
			firstBlock->next = nullptr;
			firstBlock = nullptr;

			Block* tempAddress = blockAddresses.front();
			blockAddresses.pop_front();
			delete tempAddress;
		}
		else  //delete the last element from the firstBlock
		{
			for (int i = mLastValidIndex; i > firstBlockIndex; i--)
				firstBlock->data[i] = firstBlock->data[i - 1];
			firstBlockIndex++;
		}
	}
	else  //if there is a lastBlock
	{
		if (lastBlockIndex == mInitialIndex)  //if i am popping the only element in a block,the block gets deleted and there are 2 cases:either there is another block apart from the firstBlock or there are noBlocks(or only the firstBlock) and in that case there is no longer a lastBlock
		{
			if (lastBlock->previous == firstBlock && firstBlock != nullptr)  //if the only block left is the firstBlock
			{
				Block* tempAddress = blockAddresses.back();
				blockAddresses.pop_back();
				delete tempAddress;
				lastBlock = nullptr;
				firstBlock->next = nullptr;
				lastBlockIndex = mInitialIndex - 1;
			}
			else if (lastBlock->previous != nullptr && lastBlock->previous != firstBlock)  //if there is at least one block before the LastBlock that is not the firstBlock,then the block previous to the LastBlock now becomes the lastBlock
			{
				lastBlock = lastBlock->previous;
				lastBlock->next = nullptr;
				lastBlockIndex = mLastValidIndex;
				Block* tempAddress = blockAddresses.back();
				blockAddresses.pop_back();
				delete tempAddress;
			}
			else if (lastBlock->previous == firstBlock && firstBlock == nullptr)  //if there is only one block with one element and it is being popped,then the deque becomes empty
			{
				Block* tempAddress = blockAddresses.back();
				blockAddresses.pop_back();
				delete tempAddress;
				lastBlock->previous = nullptr;
				lastBlock->next = nullptr;
				lastBlock = nullptr;
			}
		}
		else  //if i am popping an element in a block that has elements and doesn't become empty after the pop
		{
			lastBlockIndex--;
		}
	}
}

template<typename T>
void Deque<T>::pop_front()
{
	if (empty())
	{
		return;
	}
	else if (lastBlock != nullptr && firstBlock == nullptr)  //if there is only one block and it is the last block
	{
		if (lastBlockIndex == mInitialIndex)  //if there is only one element in the lastBlock and it is being deleted,the deque becomes empty
		{
			lastBlock->previous = nullptr;
			lastBlock->next = nullptr;
			lastBlock = nullptr;

			Block* tempAddress = blockAddresses.back();
			blockAddresses.pop_back();
			delete tempAddress;
		}
		else  //delete the first element from the lastBlock
		{
			for (int i = mInitialIndex; i < lastBlockIndex; i++)
				lastBlock->data[i] = lastBlock->data[i + 1];
			lastBlockIndex--;
		}
	}
	else  //if there is a firstBlock
	{
		if (firstBlockIndex == mLastValidIndex)  //if i am popping the only element in the firstBlock,it gets deleted and there are 2 cases:either there is another block apart from the lastBlock or there are no blocks(or only the lastBlock) and in that case there is no longer a firstBlock 
		{
			if (firstBlock->next == lastBlock && lastBlock != nullptr)  //if the only block left is the lastBlock
			{
				Block* tempAddress = blockAddresses.front();
				blockAddresses.pop_front();
				delete tempAddress;
				firstBlock = nullptr;
				lastBlock->previous = nullptr;
				firstBlockIndex = mBlockSize;
			}
			else if (firstBlock->next != nullptr && firstBlock->next != lastBlock)  //if there is at least one block after the firstBlock that is not the lastBlock,then the block next after the firstBlock now becomes the firstBlock
			{
				firstBlock = firstBlock->next;
				firstBlock->previous = nullptr;
				firstBlockIndex = mInitialIndex;
				Block* tempAddress = blockAddresses.front();
				blockAddresses.pop_front();
				delete tempAddress;
			}
			else if (firstBlock->next == lastBlock && lastBlock == nullptr)  //if there is only one block with one element and it is being popped,then the deque becomes empty
			{
				Block* tempAddress = blockAddresses.front();
				blockAddresses.pop_front();
				delete tempAddress;
				firstBlock->previous = nullptr;
				firstBlock->next = nullptr;
				firstBlock = nullptr;
			}
		}
		else  //if i am popping an element in a block that has elements and doesn't become empty after the pop
		{
			firstBlockIndex++;
		}
	}
}

template<typename T>
T& Deque<T>::front()
{
	if (blockAddresses.empty())
	{
		throw std::out_of_range("Deque is empty, cannot access front element.");
	}
	else
	{
		Block*& frontAddress = blockAddresses.front();
		if (firstBlock != nullptr)  //if there is a first block,the first element of the deque is at the firstBlockIndex
		{
			return frontAddress->data[firstBlockIndex];
		}
		else  //if there isn't a first block,there is only the lastBlock and the first element of the deque is at index 0 in that block
		{
			return frontAddress->data[0];
		}
	}
}

template<typename T>
T& Deque<T>::back()
{
	if (blockAddresses.empty())
	{
		throw std::out_of_range("Deque is empty, cannot access back element.");
	}
	else
	{
		Block*& backAddress = blockAddresses.back();
		if (lastBlock != nullptr)  //if there is a lastBlock
		{
			return backAddress->data[lastBlockIndex];
		}
		else  //if there is not a lastBlock,there is only the firstBlock,and the last element of the deque is at index mLastValidIndex of that block
		{
			return backAddress->data[mLastValidIndex];
		}
	}
}

template<typename T>
T& Deque<T>::operator[](int index)
{
	if (index < 0)
	{
		clear();
		throw std::out_of_range("Index out of range");
	}

	int dequeSize = size();

	if (index >= dequeSize)
	{
		if (dequeSize == 0)  //either the deque is empty
		{
			throw std::out_of_range("Trying to access empty deque");
		}
		else  //or the index is out of range
		{
			clear();
			throw std::out_of_range("Index out of range");
		}
	}

	if (firstBlock == nullptr)  //if there is only one block and it is the lastBlock
	{
		return lastBlock->data[index];
	}
	if (firstBlock != nullptr && lastBlock == nullptr)  //if there is only one block and it is the firstBlock
	{
		return firstBlock->data[firstBlockIndex + index];
	}
	else  //if there are multiple blocks
	{
		if (index <= mLastValidIndex - firstBlockIndex)  //if the element trying to be accessed is in the first block
		{
			return firstBlock->data[firstBlockIndex + index];
		}
		else  //if the element trying to be accessed in in any block apart from the first block
		{
			int firstBlockElementsCount = mBlockSize - firstBlockIndex;
			index -= firstBlockElementsCount;  //the element trying to be accessed is not in the first block so those elements are taken away from the index and the first block is ignored

			int blockIndex = index / mBlockSize;
			int elementIndex = index % mBlockSize;

			Block* block = firstBlock->next;  //the first block is ignored because the element trying to be accessed is not from the firstBlock
			for (int i = mInitialIndex; i < blockIndex; i++)
				block = block->next;

			return block->data[elementIndex];
		}
	}
}

template<typename T>
bool Deque<T>::empty()
{
	return (firstBlock == nullptr && lastBlock == nullptr);
}

template<typename T>
void Deque<T>::clear()
{
	for (auto pointer : blockAddresses)
		delete pointer;

	blockAddresses.clear();

	firstBlock = nullptr;
	lastBlock = nullptr;
	int firstBlockIndex = mBlockSize, lastBlockIndex = mInitialIndex - 1;
}

template<typename T>
int Deque<T>::size()
{
	if (empty())
		return 0;

	if (firstBlock != nullptr && lastBlock == nullptr)  //if there is only the firstBlock
		return mBlockSize - firstBlockIndex;

	if (lastBlock != nullptr && firstBlock == nullptr)  //if there is only the lastBlock
		return lastBlockIndex + 1;

	int count = 0;
	if (blockAddresses.size() >= 2)  //if there are at least a first block and a last block
	{
		for (int i = firstBlockIndex; i < mBlockSize; i++)
			count++;

		Block* pointer = blockAddresses.front();
		pointer = pointer->next;
		for (; pointer != blockAddresses.back(); pointer = pointer->next)
			count += mBlockSize;

		for (int i = mInitialIndex; i <= lastBlockIndex; i++)
			count++;
	}

	return count;
}

template<typename T>
void Deque<T>::insert(const T& value, int index)
{
	int dequeSize = size();
	if (index < 0 || index > dequeSize)  //invalid index handling
	{
		clear();
		throw std::out_of_range("Index out of range");
	}

	if (index == mInitialIndex)  //if the value is inserted at index 0,then it is just push_front
		push_front(value);
	else if (index == dequeSize) //if the value is inserted after the last index,then it is just push_back
		push_back(value);
	else  //the value is inserted at the index and the elements after it are shifted one index to the right
	{
		T lastValue = (*this)[dequeSize - 1];  //the last element is saved and pushed_back.This way it is shifted to the right and also covers the case that a new block has to be made
		for (int i = dequeSize - 1; i > index; i--)
			(*this)[i] = (*this)[i - 1];
		(*this)[index] = value;
		push_back(lastValue);
	}
}

template<typename T>
void Deque<T>::remove(int index)
{
	int dequeSize = size();
	if (index < 0 || index >= dequeSize)  //invalid index handling
	{
		clear();
		throw std::out_of_range("Index out of range");
	}

	if (index == mInitialIndex)  //if the first element is removed
		pop_front();
	else if (index == dequeSize - 1)  //if the last element is removed
		pop_back();
	else
	{
		for (int i = index; i < dequeSize - 1; i++)  //shift all the elements one index to the left
			(*this)[i] = (*this)[i + 1];
		pop_back();  //remove the extra element
	}
}

template<typename T>
typename Deque<T>::Iterator Deque<T>::begin()
{
	Iterator it(*this);
	if (firstBlock != nullptr)
	{
		it.block = firstBlock;
		it.blockIndex = firstBlockIndex;
		return it;
	}
	if (lastBlock != nullptr)
	{
		it.block = lastBlock;
		it.blockIndex = mInitialIndex;
	}

	return it;
}

template<typename T>
typename Deque<T>::Iterator Deque<T>::end()
{
	Iterator it(*this);
	if (lastBlock != nullptr)
	{
		it.block = lastBlock;
		if (lastBlockIndex < mBlockSize)
			it.blockIndex = lastBlockIndex + 1;
		else
			it.blockIndex = mBlockSize;
		return it;
	}
	if (firstBlock != nullptr)
	{
		it.block = firstBlock;
		it.blockIndex = mBlockSize;
	}

	return it;
}

template<typename T>
T& Deque<T>::Iterator::operator*()
{
	if (block == nullptr || blockIndex < mInitialIndex || blockIndex >= mBlockSize)
		throw std::runtime_error("Invalid iterator dereference");

	return block->data[blockIndex];
}

template<typename T>
T* Deque<T>::Iterator::operator->()
{
	if (block == nullptr || blockIndex < mInitialIndex || blockIndex >= mBlockSize)
		throw std::runtime_error("Invalid iterator dereference");

	return &(block->data[blockIndex]);
}

template<typename T>
typename Deque<T>::Iterator& Deque<T>::Iterator::operator=(const Iterator& other)
{
	block = other.block;
	blockIndex = other.blockIndex;
	this->dequeReference = other.dequeReference;

	return *this;
}

template<typename T>
bool Deque<T>::Iterator::operator==(const Iterator& other)
{
	return(dequeReference.firstBlock == other.dequeReference.firstBlock && dequeReference.lastBlock == other.dequeReference.lastBlock && block == other.block && blockIndex == other.blockIndex);
}

template<typename T>
bool Deque<T>::Iterator::operator!=(const Iterator& other)
{
	return !(*this == other);
}

template<typename T>
typename Deque<T>::Iterator& Deque<T>::Iterator::operator++()
{
	if (block == nullptr)
		throw std::runtime_error("Attempted to access nullptr");

	if (blockIndex < mLastValidIndex)
	{
		++blockIndex;
		return *this;
	}

	if (block->next == nullptr || block == this->dequeReference.blockAddresses.back())
	{
		*this = dequeReference.end();
		return *this;
	}

	blockIndex = mInitialIndex;
	block = block->next;
	return *this;
}

template<typename T>
typename Deque<T>::Iterator& Deque<T>::Iterator::operator--()
{
	if (block == nullptr)
		throw std::runtime_error("Attempted to access nullptr");

	if (block == dequeReference.firstBlock)
	{
		if (blockIndex <= dequeReference.firstBlockIndex)
		{
			*this = dequeReference.begin();
			return *this;
		}

		--blockIndex;
		return *this;
	}
	if (blockIndex <= mInitialIndex)
	{
		block = block->previous;
		blockIndex = mLastValidIndex;
		return *this;
	}

	--blockIndex;
	return *this;
}

template<typename T>
typename Deque<T>::Iterator Deque<T>::Iterator::operator++(int)
{
	Iterator temp = *this;
	++(*this);

	return temp;
}

template<typename T>
typename Deque<T>::Iterator Deque<T>::Iterator::operator--(int)
{
	Iterator temp = *this;
	--(*this);

	return temp;
}

template<typename T>
typename Deque<T>::Iterator Deque<T>::Iterator::operator+(int stepsForward)
{
	Iterator it = *this;
	for (int i = 0; i < stepsForward; ++i)
		++it;

	return it;
}

template<typename T>
typename Deque<T>::Iterator Deque<T>::Iterator::operator-(int stepsBackwards)
{
	Iterator it = *this;
	for (int i = 0; i < stepsBackwards; i++)
		--it;

	return it;
}

template<typename T>
typename Deque<T>::Iterator& Deque<T>::Iterator::operator+=(int steps)
{
	while (steps > 0)
	{
		++(*this);
		--steps;
	}
	while (steps < 0)  //if steps value is negative,it goes backwards
	{
		--(*this);
		++steps;
	}

	return *this;
}

template<typename T>
typename Deque<T>::Iterator& Deque<T>::Iterator::operator-=(int steps)
{
	while (steps > 0)
	{
		--(*this);
		--steps;
	}
	while (steps < 0)
	{
		++(*this);
		++steps;
	}

	return *this;
}
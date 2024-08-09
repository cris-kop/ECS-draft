#include <vector>

struct ComponentStorage
{
	virtual ~ComponentStorage() { };

	virtual unsigned int CopyComponentFromOtherStorage(ComponentStorage *pSourceStorage, unsigned int pSourceIndex) = 0;
	virtual void CopyAllComponentsFromOtherStorage(ComponentStorage *pSourceStorage) = 0;

	virtual bool SetEntityId(const uint32_t pIndex, const uint32_t pEntityId) = 0;
	virtual void DeleteComponent(const unsigned int pIndex) = 0;

	virtual size_t GetSize() = 0;
};

template<typename T>
struct ActualStorage : ComponentStorage
{
	std::vector<T>	actualVector;
	
	unsigned int CopyComponentFromOtherStorage(ComponentStorage *pSourceStorage, unsigned int pSourceIndex)
	{
		ActualStorage<T> *sourceStorage = static_cast<ActualStorage<T>*>(pSourceStorage);		

		if(pSourceIndex >= sourceStorage->actualVector.size())
		{
			return -1;
		}

		actualVector.emplace_back(sourceStorage->actualVector[pSourceIndex]);
		return static_cast<unsigned int>(actualVector.size()) - 1;
	}

	void CopyAllComponentsFromOtherStorage(ComponentStorage *pSourceStorage)
	{
		ActualStorage<T> *sourceStorage = static_cast<ActualStorage<T>*>(pSourceStorage);
		actualVector.reserve(sourceStorage->actualVector.size());

		actualVector.insert(actualVector.end(), sourceStorage->actualVector.begin(), sourceStorage->actualVector.end());
	}

	bool SetEntityId(const uint32_t pIndex, const uint32_t pEntityId)
	{
		if(pIndex >= actualVector.size())
		{
			return false;
		}
		actualVector[pIndex].ChangeEntityId(pEntityId);
		return true;
	}

	void DeleteComponent(const unsigned int pIndex)
	{
		if(pIndex < actualVector.size() - 1)
		{
			actualVector[pIndex] = actualVector.back();
		}
		actualVector.pop_back();
	}

	size_t GetSize()
	{
		return actualVector.size();
	}
};

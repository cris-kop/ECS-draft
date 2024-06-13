#ifndef ENTITY_H
#define ENTITY_H

#include "ComponentSet.h"

struct Entity
{
	Entity() : mGlobalId(-1), mRowIndex(-1) { }
	Entity(const unsigned int pGlobalId) : mGlobalId(pGlobalId), mRowIndex(-1) { }
	Entity(const unsigned int pGlobalId, const unsigned int pRowIndex, const ComponentSet pComponentSet) : mGlobalId(pGlobalId), mRowIndex(pRowIndex), mComponentSet(pComponentSet) { }

	Entity(const Entity& pOther)						// copy constructor
	{
		mGlobalId = -1;
		mRowIndex = pOther.GetRowIndex();
		mComponentSet = pOther.GetComponentSet();
	}
	Entity(Entity&& pOther) = delete;					// move constructor
	Entity& operator=(const Entity& other) = delete;	// copy assignment
	Entity& operator=(Entity&& other) = delete;			// move assignment
	
	ComponentSet GetComponentSet()	const	{	return mComponentSet;	}
	int GetRowIndex()				const	{	return mRowIndex;		}
	int GetGlobalId()				const	{	return mGlobalId;		}

	void SetRowIndex(const unsigned int pIndex)			{	mRowIndex = pIndex;	}
	void SetComponentSet(const ComponentSet pNewSet)	{	mComponentSet = pNewSet; }

	ComponentSet mComponentSet = ComponentSet::None;

	int mGlobalId;
	int	mRowIndex;
};

#endif
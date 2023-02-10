#ifndef ENTITY_H
#define ENTITY_H

#include "ComponentSet.h"

struct Entity
{
	Entity() : mGlobalId(-1) { }
	Entity(const unsigned int pGlobalId) : mGlobalId(pGlobalId), mRowIndex(-1) { }
	
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
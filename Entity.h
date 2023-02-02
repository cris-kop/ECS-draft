#ifndef ENTITY_H
#define ENTITY_H

#include "ComponentSet.h"

struct Entity
{
	Entity(const unsigned int pGlobalId) : mGlobalIndex(pGlobalId), mRowIndex(-1) { }
	
	ComponentSet GetComponentSet()		{	return mComponentSet;	}
	int GetRowIndex()					{	return mRowIndex;		}

	void SetRowIndex(const unsigned int pIndex)			{	mRowIndex = pIndex;	}
	void SetComponentSet(const ComponentSet pNewSet)	{	mComponentSet = pNewSet; }
	void SetGlobalIndex(const unsigned int pIndex)		{	mGlobalIndex = pIndex;	}

	ComponentSet mComponentSet = ComponentSet::None;

	size_t	mGlobalIndex;
	int		mRowIndex;
};

#endif
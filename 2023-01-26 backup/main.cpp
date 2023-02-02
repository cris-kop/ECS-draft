#include <Windows.h>
#include <string>
#include <vector>
#include <bitset>
#include <unordered_map>
#include "main.h"
#include "ComponentSet.h"

enum Archetypes
{
	WorldProp,		// 0
	Camera			// 1
};

std::unordered_map<Archetypes, ComponentSet> ArchetypeMap =
{
	{ Archetypes::WorldProp, ComponentSet::Transform },
	{ Archetypes::Camera, ComponentSet::Transform | ComponentSet::Camera }
};


struct Entity
{
	Entity(const Archetypes pArchetype, const unsigned int pGlobalId, const unsigned int pDataVectorIndex) : 
		mArchetype(pArchetype), mGlobalId(pGlobalId), mDataVectorIndex(pDataVectorIndex) { }
	
	Archetypes mArchetype;

	size_t mGlobalId;
	size_t mDataVectorIndex;
};

struct TransformComponent
{
	Vector3f mPos;
	Vector3f mRot;
	Vector3f mScale;

	Vector3f mPosRotScale;	// just illustrative

	TransformComponent() { }
	TransformComponent(const Vector3f &pPos, const Vector3f &pRot, const Vector3f &pScale)
		: mPos(pPos), mRot(pRot), mScale (pScale) { }
};

struct CameraComponent
{
	Vector3f mLookAt;
	Vector3f mYawPitchRoll;

	Vector3f mLookAtPlusPosRotScale;		// just illustrative

	CameraComponent() { }
	CameraComponent(const Vector3f &pLookAt, const Vector3f &pYawPitchRoll)
		: mLookAt(pLookAt), mYawPitchRoll(pYawPitchRoll) { }
};

struct ArchetypeData
{
	ArchetypeData(const Archetypes &pArchetype) : mArchetype(ArchetypeMap[pArchetype]) { }
	
	std::vector<TransformComponent>	mTransforms;
	std::vector<CameraComponent>	mCameras;

	ComponentSet mArchetype;
};

struct ISystem
{
	ISystem(const Archetypes pArcheType) : mArchetype(ArchetypeMap[pArcheType]) { }

	virtual void Update(ArchetypeData &pComponentsData) = 0;

	ComponentSet mArchetype;
};


struct WorldPropsSystem : public ISystem
{
	WorldPropsSystem(const Archetypes pArcheType) : ISystem(pArcheType) { }

	void Update(ArchetypeData &pComponentsData)
	{
		for(auto && transform : pComponentsData.mTransforms)
		{
			transform.mPosRotScale = transform.mPos + transform.mRot + transform.mScale;
		}
	}
};

struct CameraSystem : public ISystem
{
	CameraSystem(const Archetypes pArcheType) : ISystem(pArcheType) { }

	void Update(ArchetypeData &pComponentsData)
	{
		for(size_t index=0;index<pComponentsData.mCameras.size();++index)
		{
			pComponentsData.mCameras[index].mLookAtPlusPosRotScale = pComponentsData.mCameras[index].mLookAt + pComponentsData.mTransforms[index].mPosRotScale;
		}
	}
};

struct EntityAdmin
{
	std::vector<Entity>				mEntities;
	std::vector<ArchetypeData>		mArchetypesData;

	std::vector<ISystem*>			mSystems;
	
	void Init()
	{
		mArchetypesData.emplace_back(Archetypes::WorldProp);
		mArchetypesData.emplace_back(Archetypes::Camera);

		ISystem *worldPropSystem = new WorldPropsSystem(Archetypes::WorldProp);
		ISystem *cameraSystem = new CameraSystem(Archetypes::Camera);

		mSystems.emplace_back(worldPropSystem);
		mSystems.emplace_back(cameraSystem);
	}

	int AddWorldProp(const Vector3f &pPos, const Vector3f &pRot, const Vector3f &pScale)
	{
		TransformComponent myTransform(pPos, pRot, pScale);
		mArchetypesData[Archetypes::WorldProp].mTransforms.emplace_back(myTransform);

		mEntities.emplace_back(Archetypes::WorldProp, mEntities.size()-1, mArchetypesData[Archetypes::WorldProp].mTransforms.size() - 1);
		return static_cast<int>(mEntities.size()) - 1;
	}
	int AddCamera(const Vector3f &pPos, const Vector3f &pRot, const Vector3f &pScale, const Vector3f &pLookAt, const Vector3f &pYawPitchRoll)
	{
		CameraComponent myCamera(pLookAt, pYawPitchRoll);
		TransformComponent myTransform(pPos, pRot, pScale);
		mArchetypesData[Archetypes::Camera].mCameras.emplace_back(myCamera);
		mArchetypesData[Archetypes::Camera].mTransforms.emplace_back(myTransform);

		mEntities.emplace_back(Archetypes::Camera, mEntities.size() - 1, mArchetypesData[Archetypes::WorldProp].mTransforms.size() - 1);
		return static_cast<int>(mEntities.size()) - 1;
	}

	void UpdateSystems()
	{
		for(auto && archeType : mArchetypesData)
		{
			for(auto && system : mSystems)
			{
				if(Contains(system->mArchetype, archeType.mArchetype))
				{
					system->Update(archeType);
				}
			}
		}
	}

	TransformComponent GetTransformComponent(const unsigned int pEntityIndex)
	{
		if(pEntityIndex < static_cast<unsigned int>(mEntities.size()))
		{
			auto archetype = mEntities[pEntityIndex].mArchetype;
			if(Contains(ArchetypeMap[archetype], ComponentSet::Transform))
			{
				return mArchetypesData[archetype].mTransforms[mEntities[pEntityIndex].mDataVectorIndex];
			}
		}
		return TransformComponent();
	}
};



int WINAPI WinMain(HINSTANCE hInstance,
				   HINSTANCE hPrevInstance,
				   LPSTR lpCmdLine,
				   int nCmdShow)
{
	EntityAdmin myAdmin;
	myAdmin.Init();

	myAdmin.AddWorldProp(Vector3f(0.0f, 5.0f, 1.0f), Vector3f(90.0f, 0.0f, 0.0f), Vector3f(1.0f, 1.5f, 1.0f));
	myAdmin.AddWorldProp(Vector3f(1.0f, 5.0f, 3.0f), Vector3f(0.0f, 90.0f, 0.0f), Vector3f(1.0f, 1.0f, 0.5f));
	int entityId = myAdmin.AddWorldProp(Vector3f(2.0f, 5.0f, 5.0f), Vector3f(0.0f, 0.0f, 90.0f), Vector3f(0.75f, 1.0f, 1.5f));

	myAdmin.AddCamera(Vector3f(1.0f, 5.0f, 3.0f), Vector3f(0.0f, 90.0f, 0.0f), Vector3f(1.0f, 1.0f, 0.5f), Vector3f(0.0f, 0.0f, 1.0f), Vector3f(0.0f, 45.0f, 0.0f));

	myAdmin.UpdateSystems();

	TransformComponent testGetter = myAdmin.GetTransformComponent(entityId);

	return 0;
}

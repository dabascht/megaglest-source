// ==============================================================
//	This file is part of Glest (www.glest.org)
//
//	Copyright (C) 2001-2008 Martiño Figueroa
//
//	You can redistribute this code and/or modify it under 
//	the terms of the GNU General Public License as published 
//	by the Free Software Foundation; either version 2 of the 
//	License, or (at your option) any later version
// ==============================================================
#ifndef _GLEST_GAME_OBJECTTYPE_H_
#define _GLEST_GAME_OBJECTTYPE_H_

#ifdef WIN32
    #include <winsock2.h>
#endif

#include <vector>
#include "model.h"
#include "vec.h"
#include "leak_dumper.h"
#include "unit_particle_type.h"
#include "tileset_model_type.h"

using std::vector;

namespace Glest{ namespace Game{

using Shared::Graphics::Model;
using Shared::Graphics::Vec3f;

// =====================================================
// 	class ObjectType  
//
///	Each of the possible objects of the map: trees, stones ...
// =====================================================

typedef vector<ObjectParticleSystemType*> ObjectParticleSystemTypes;
typedef vector<ObjectParticleSystemTypes> ObjectParticleVector;

class ObjectType {
private:
	typedef vector<TilesetModelType*> ModelTypes;
private:
	static const int tree1= 0;
	static const int tree2= 1;
	static const int choppedTree= 2;

private:
	ModelTypes modeltypes;
	Vec3f color;
	int objectClass;
	bool walkable;
	int height;

public:
	ObjectType() {
		objectClass = -1;
		walkable = false;
		height = 0;
	}
	~ObjectType();
	void init(int modelCount, int objectClass, bool walkable, int height);

	TilesetModelType* loadModel(const string &path, std::map<string,vector<pair<string, string> > > *loadedFileList=NULL,
			string parentLoader="");

	inline TilesetModelType *getTilesetModelType(int i)			{return modeltypes[i];}
	inline int getModelCount() const		{return (int)modeltypes.size();}
	inline const Vec3f &getColor() const	{return color;}
	inline int getClass() const			{return objectClass;}
	inline bool getWalkable() const		{return walkable;}
	inline int getHeight() const			{return height;}
	inline bool isATree() const			{return objectClass==tree1 || objectClass==tree2;}
	void deletePixels();
};

}}//end namespace

#endif

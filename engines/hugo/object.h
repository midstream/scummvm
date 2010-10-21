/* ScummVM - Graphic Adventure Engine
 *
 * ScummVM is the legal property of its developers, whose names
 * are too numerous to list here. Please refer to the COPYRIGHT
 * file distributed with this source distribution.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.

 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.

 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 *
 * $URL$
 * $Id$
 *
 */

/*
 * This code is based on original Hugo Trilogy source code
 *
 * Copyright (c) 1989-1995 David P. Gray
 *
 */

#ifndef HUGO_OBJECT_H
#define HUGO_OBJECT_H

#include "common/file.h"

#define MAXOBJECTS      128                         // Used in Update_images()

namespace Hugo {

class ObjectHandler {
public:
	ObjectHandler(HugoEngine *vm);
	~ObjectHandler();

	object_t  *_objects;
	
	bool  isCarrying(uint16 wordIndex);

	int16 findObject(uint16 x, uint16 y);

	void lookObject(object_t *obj);
	void freeObjects();
	void loadObject(Common::File &in);
	void moveObjects();
	void restoreSeq(object_t *obj);
	void saveSeq(object_t *obj);
	void showTakeables();
	void swapImages(int objNumb1, int objNumb2);
	void updateImages();
	void useObject(int16 objId);
	
	static int y2comp(const void *a, const void *b);

	bool isCarried(int objIndex) {
		return _objects[objIndex].carriedFl;
	}

	void setCarry(int objIndex, bool val) {
		_objects[objIndex].carriedFl = val;
	}
	
	void setVelocity(int objIndex, int8 vx, int8 vy) {
		_objects[objIndex].vx = vx;
		_objects[objIndex].vy = vy;
	}

	void setPath(int objIndex, path_t pathType, int16 vxPath, int16 vyPath) {
		_objects[objIndex].pathType = pathType;
		_objects[objIndex].vxPath = vxPath;
		_objects[objIndex].vyPath = vyPath;
	}
private:
	HugoEngine *_vm;
};

} // End of namespace Hugo

#endif //HUGO_OBJECT_H

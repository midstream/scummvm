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
 */

#ifndef ASYLUM_BOARDYOUTH_H
#define ASYLUM_BOARDYOUTH_H

#include "asylum/puzzles/board.h"

namespace Asylum {

class AsylumEngine;

class PuzzleBoardYouth : public PuzzleBoard {
public:
	PuzzleBoardYouth(AsylumEngine *engine);

	// Serializable
	virtual void saveLoadWithSerializer(Common::Serializer &s);

private:
	//////////////////////////////////////////////////////////////////////////
	// Event Handling
	//////////////////////////////////////////////////////////////////////////
	bool mouseLeftDown(const AsylumEvent &evt);
};

} // End of namespace Asylum

#endif // ASYLUM_BOARDYOUTH_H

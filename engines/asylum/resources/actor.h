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

#ifndef ASYLUM_ACTOR_H
#define ASYLUM_ACTOR_H

#include "asylum/shared.h"

#include "common/array.h"
#include "common/rect.h"
#include "common/serializer.h"
#include "common/stream.h"

namespace Asylum {

class AsylumEngine;
class GraphicResource;
class Screen;

struct ActionArea;
struct GraphicFrame;

struct ActorData : public Common::Serializable {
	uint32 count;
	int32 current;
	Common::Point points[120];
	ActorDirection directions[120];

	void load(Common::SeekableReadStream *stream) {
		count = stream->readUint32LE();

		current = stream->readUint32LE();

		for (int32 i = 0; i < 120; i++) {
			points[i].x = stream->readSint32LE();
			points[i].y = stream->readSint32LE();
		}

		for (int32 i = 0; i < 120; i++)
			directions[i] = (ActorDirection)stream->readSint32LE();
	}

	// Serializable
	void saveLoadWithSerializer(Common::Serializer &s) {
		s.syncAsUint32LE(count);
		s.syncAsSint32LE(current);

		for (int32 i = 0; i < ARRAYSIZE(points); i++) {
			s.syncAsSint32LE(points[i].x);
			s.syncAsSint32LE(points[i].y);
		}

		for (int32 i = 0; i < ARRAYSIZE(directions); i++)
			s.syncAsSint32LE(directions[i]);
	}
};

class Actor : public Common::Serializable {
public:
	Actor(AsylumEngine *engine, ActorIndex index);
	virtual ~Actor();

	//////////////////////////////////////////////////////////////////////////
	// Public variables & accessors
	//////////////////////////////////////////////////////////////////////////
	int32 flags;
	int32 actionType; // ActionType enum value


	void setActionIndex2(int32 index) { _actionIdx2 = index; }
	void setObjectIndex(int32 index) { _objectIndex = index; }
	void setDirection(ActorDirection dir) { _direction = dir; }
	void setFrameCount(int32 count) { _frameCount = count; }
	void setFrameIndex(int32 number) { _frameIndex = number; }
	void setLastScreenUpdate(int32 tick) { _lastScreenUpdate = tick; }
	void setNumberFlag01(int32 number) { _numberFlag01 = number; }
	void setPriority(int32 priority) { _priority = priority; }
	void setReaction(int32 index, int32 val) { _reaction[index] = val; }
	void setResourceId(ResourceId id) { _resourceId = id; }
	void setSoundResourceId(ResourceId id) { _soundResourceId = id; }
	void setStatus(ActorStatus status) { _status = status; }
	void setTransparency(int32 val) { _transparency = val; }

	void setField638(int32 val) { _field_638 = val; }
	void setField934(int32 val) { _field_934 = val; }
	void setField938(int32 val) { _field_938 = val; }
	void setField944(int32 val) { _field_944 = val; }

	int32          getActionIndex3() { return _actionIdx3; }
	Common::Rect  *getBoundingRect() { return &_boundingRect; }
	ActorDirection getDirection() { return _direction; }
	uint32         getFrameCount() { return _frameCount; }
	uint32         getFrameIndex() { return _frameIndex; }
	char          *getName() { return (char *)&_name; }
	ActorIndex     getNextActorIndex() { return _nextActorIndex; }
	int32          getNumberValue01() { return _numberValue01; }
	Common::Point *getPoint() { return &_point; }
	Common::Point *getPoint1() { return &_point1; }
	Common::Point *getPoint2() { return &_point2; }
	int32          getPriority() { return _priority; }
	int32          getReactionValue(uint32 index) { return _reaction[index]; }
	ResourceId     getResourceId() { return _resourceId; }
	ResourceId     getResourcesId(uint32 index) { return _graphicResourceIds[index]; }
	int32          getScriptIndex() { return _scriptIndex; }
	bool           shouldInvertPriority() { return _invertPriority; }
	ResourceId     getSoundResourceId() { return _soundResourceId; }
	ActorStatus    getStatus()    { return _status; }

	int32          getField638() { return _field_638; }
	int32          getField934() { return _field_934; }
	int32          getField944() { return _field_944; }
	int32          getField948() { return _field_948; }
	int32          getField94C() { return _field_94C; }

	// For saving
	ActorData     *getData() { return &_data; }

	/////////////////////////////////////////////////////////////////////////
	// Data
	/////////////////////////////////////////////////////////////////////////

	/**
	 * Loads the actor
	 *
	 * @param stream If non-null, the Common::SeekableReadStream to load from
	 */
	void load(Common::SeekableReadStream *stream);

	/////////////////////////////////////////////////////////////////////////
	// Visibility
	/////////////////////////////////////////////////////////////////////////

	/**
	 * Query if this actor is visible.
	 *
	 * @return true if visible, false if not.
	 */
	bool isVisible() { return flags & kActorFlagVisible; }

	/**
	 * Query if this actor is on screen.
	 *
	 * @return true if on screen, false if not.
	 */
	bool isOnScreen();

	/**
	 * Shows this actor.
	 */
	void show() { setVisible(true); }

	/**
	 * Hides this actor.
	 */
	void hide() { setVisible(false); }

	/////////////////////////////////////////////////////////////////////////
	// Drawing & update
	//////////////////////////////////////////////////////////////////////////

	/**
	 * Draws the actor
	 */
	void draw();

	/**
	 * Draw number text (called on scene drawing)
	 */
	void drawNumber();

	/**
	 * Updates the actor.
	 */
	void update();

	/**
	 * Enables the actor
	 */
	void enable() { updateStatus(kActorStatusEnabled); }

	/**
	 * Updates the actor status.
	 *
	 * @param status The status.
	 */
	void updateStatus(ActorStatus status);

	/////////////////////////////////////////////////////////////////////////
	// Direction & position
	/////////////////////////////////////////////////////////////////////////

	/**
	 * Updates the actor direction, using the values set by script
	 */
	void updateDirection();

	/**
	 * Updates resource Id using the actor direction.
	 *
	 * @param direction The direction.
	 */
	void updateFromDirection(ActorDirection direction);

	/**
	 * Face a target from a certain direction
	 *
	 * @param target Identifier for the target.
	 * @param from   direction to face from.
	 */
	void faceTarget(uint32 target, DirectionFrom from);

	/**
	 * Initialize the x1/y1 values of the actor, update the active animation frame and, if the current direction isn't 8, update the actor's direction.
	 *
	 * @param newX 		   The new x coordinate.
	 * @param newY 		   The new y coordinate.
	 * @param newDirection The new direction.
	 * @param frame 	   The frame.
	 */
	void setPosition(int32 newX, int32 newY, ActorDirection newDirection, uint32 frame);

	/**
	 * Query if the passed direction is default direction.
	 *
	 * @param index Zero-based index of the graphic resource.
	 *
	 * @return true if default direction, false if not.
	 */
	bool isDefaultDirection(int index) const;

	/**
	 * Adjust coordinates.
	 *
	 * @param point The point.
	 */
	void adjustCoordinates(Common::Point *point);

	/////////////////////////////////////////////////////////////////////////
	// Misc
	/////////////////////////////////////////////////////////////////////////

	/**
	 * Stop the actor related sounds
	 */
	void stopSound();

	/**
	 * Convert this object into a string representation.
	 *
	 * @param shortString toggle whether to output a summary or
	 *                    detailed view of the actor object
	 * @return A string representation of this object.
	 */
	Common::String toString(bool shortString = true);

	/**
	 * Clears actor data fields
	 */
	void clearFields();

	// Unknown methods
	bool process(const Common::Point &point);
	void processStatus(int32 actorX, int32 actorY, bool doSpeech);
	void processNext(ActorIndex nextActor, int32 actionAreaId, ActorDirection nextDirection, const Common::Point &nextPosition, bool invertPriority, const Common::Point &nextPositionOffset);
	bool canInteract(Common::Point *point, int32* param);
	bool canMove(Common::Point *point, ActorDirection direction, uint32 count, bool hasDelta);
	void move(ActorDirection dir, uint32 distance);
	void addReactionHive(int32 reactionIndex, int32 numberValue01Add);
	void removeReactionHive(int32 reactionIndex, int32 numberValue01Substract);
	bool hasMoreReactions(int32 reactionIndex, int32 testNumberValue01);
	bool canMoveCheckActors(Common::Point *point, ActorDirection direction);
	void updateAndDraw();
	void update_409230();

	/**
	 * Query if the object resource is present in the resource table between indices 10 & 20
	 *
	 * @return true if resource present between 15 & 20, false if not.
	 */
	bool isResourcePresent() const;

	//////////////////////////////////////////////////////////////////////////
	// Static update methods
	//////////////////////////////////////////////////////////////////////////
	static void enableActorsChapter2(AsylumEngine *engine);

	/**
	 * Updates the player appearance in chapter 9.
	 *
	 * @param [in,out] engine If non-null, the engine.
	 * @param nextPlayer 	  The next player index
	 */
	static void updatePlayerChapter9(AsylumEngine *engine, int nextPlayer);

	/**
	 * Gets a direction using the angle between the two points.
	 *
	 * @param vec1 The first vector.
	 * @param vec2 The second vector.
	 *
	 * @return The direction
	 */
	static ActorDirection directionFromAngle(Common::Point vec1, Common::Point vec2);

	/**
	 * Get the euclidean distance between the two vectors
	 *
	 * @param vec1 The first vector.
	 * @param vec2 The second vector.
	 *
	 * @return the distance.
	 */
	static uint32 euclidianDistance(Common::Point vec1, Common::Point vec2);

	// Serializable
	void saveLoadWithSerializer(Common::Serializer &s);

private:
	AsylumEngine *_vm;

	// Our current index
	ActorIndex _index;

	//////////////////////////////////////////////////////////////////////////
	// Data
	//////////////////////////////////////////////////////////////////////////
	Common::Point _point;
	ResourceId _resourceId;
	int32  _objectIndex;
	uint32 _frameIndex;
	uint32 _frameCount;
	Common::Point _point1;
	Common::Point _point2;
	Common::Rect _boundingRect;
	ActorDirection _direction;
	int32  _field_3C;
	ActorStatus _status;
	int32  _field_44;
	int32  _priority;
	//flags
	int32  _field_50;
	int32  _field_54;
	int32  _field_58;
	int32  _field_5C;
	int32  _field_60;
	int32  _actionIdx3;
	// TODO field_68 till field_617
	int32  _reaction[8];
	int32  _field_638;
	ResourceId _walkingSound1;
	ResourceId _walkingSound2;
	ResourceId _walkingSound3;
	ResourceId _walkingSound4;
	uint32  _field_64C;
	uint32  _field_650;
	ResourceId  _graphicResourceIds[55];
	char   _name[256];
	int32  _distancesEO[20];
	int32  _distancesNS[20];
	int32  _distancesNSEO[20];
	int32  _actionIdx2;
	int32  _field_924;
	uint32 _lastScreenUpdate;
	int32  _scriptIndex;
	//actionType
	int32  _field_934;
	int32  _field_938;
	ResourceId _soundResourceId; // field_93C
	int32  _numberValue01;
	int32  _field_944;
	int32  _field_948;
	int32  _field_94C;
	int32  _numberFlag01;
	int32  _numberStringWidth;
	int32  _numberStringX;
	int32  _numberStringY;
	char   _numberString01[8];
	int32  _field_968;
	int32  _transparency;
	bool   _processNewDirection;
	bool   _invertPriority;
	ActorDirection _nextDirection;
	int32  _nextActionIndex;
	ActorIndex _nextActorIndex;
	Common::Point _nextPositionOffset;
	Common::Point _nextPosition;
	int32  _field_994;
	int32  _field_998;
	int32  _field_99C;
	int32  _field_9A0;

	//////////////////////////////////////////////////////////////////////////
	// Actor data
	//////////////////////////////////////////////////////////////////////////
	ActorData _data;

	int32 _tickCount;

	uint32 _updateCounter;

	//////////////////////////////////////////////////////////////////////////
	// Update methods
	//////////////////////////////////////////////////////////////////////////
	void updateStatus3_19();
	void updateStatus19_Player();
	void updatePumpkin(GameFlag flagToCheck, GameFlag flagToSet, ObjectId objectToUpdate, ObjectId objectToDisable);

	void updateStatusEnabled();
	void updateStatusEnabledProcessStatus(int32 testX, int32 testY, uint32 counter, int32 setX, int32 setY);

	void updateStatus9();

	void updateStatus12_Chapter2();
	void updateStatus12_Chapter2_Actor11();
	void updateStatus12_Chapter11_Actor1();
	void updateStatus12_Chapter11();

	void updateStatus14();
	void updateStatus14_Chapter2();
	void updateStatus14_Chapter11();

	void updateStatus15_Chapter2();
	void updateStatus15_Chapter2_Helper();
	void updateStatus15_Chapter2_Player();
	void updateStatus15_Chapter2_Player_Helper();
	bool updateStatus15_isNoVisibleOrStatus17();
	void updateStatus15_Chapter2_Actor11();
	bool updateStatus15_Chapter2_Actor11_Helper(ActorIndex actorIndex1, ActorIndex actorIndex2);
	void updateStatus15_Chapter11();
	void updateStatus15_Chapter11_Player();

	void updateStatus16_Chapter2();
	void updateStatus16_Chapter11();

	void updateStatus17_Chapter2();

	void updateStatus18_Chapter2();
	void updateStatus18_Chapter2_Actor11();

	void updateStatus21();

	void updateFinish();

	//////////////////////////////////////////////////////////////////////////
	// Path finding functions
	//////////////////////////////////////////////////////////////////////////
	uint32 _frameNumber;
	bool processActionLeft(Common::Point source, const Common::Point &destination, Common::Array<int> *actions);
	bool processActionAll(Common::Point source,  const Common::Point &destination, Common::Array<int> *actions);
	bool processActionTop(Common::Point source,  const Common::Point &destination, Common::Array<int> *actions);
	bool processActionDown(Common::Point source, const Common::Point &destination, Common::Array<int> *actions);
	bool processAction(const Common::Point &source, Common::Array<int> *actions, Common::Point *point, ActorDirection direction, const Common::Point &destination, bool *flag);
	bool checkPath(Common::Array<int> *actions, const Common::Point &point, ActorDirection direction, uint32 loopcount);
	bool checkAllActions(const Common::Point &pt, Common::Array<int> *actions);

	//////////////////////////////////////////////////////////////////////////
	// Misc
	//////////////////////////////////////////////////////////////////////////

 	/**
	 * Sets actor visibility
	 *
	 * @param value true to set to visible, false to set to hidden.
	 */
	void setVisible(bool value);

	/**
	 * Sets the volume.
	 */
	void setVolume();

	/**
	 * Updates the coordinates.
	 *
	 * @param vec1 The first vector.
	 * @param vec2 The second vector.
	 */
	void updateCoordinates(Common::Point vec1, Common::Point vec2);

	/**
	 * Hide Actor 0 and reset Actor 1 frame index
	 */
	void resetActors();

	/**
	 * Updates the actor "number" data if the reaction is "1".
	 *
	 * @param reaction The reaction.
	 * @param point    The coordinates
	 */
	void updateNumbers(int32 reaction, const Common::Point &point);

	/**
	 * Determine if the supplied point is in the action area
	 *
	 * @param pt 			The point.
	 * @param [in,out] area If non-null, the area.
	 *
	 * @return true if in the action area, false otherwise
	 */
	bool isInActionArea(const Common::Point &pt, ActionArea *area);

	//////////////////////////////////////////////////////////////////////////
	// Helper methods
	//////////////////////////////////////////////////////////////////////////

	/**
	 * Updates the actor graphic information
	 *
	 * @param offset The offset used to get the id from the _graphicResourceIds table
	 */
	void updateGraphicData(uint32 offset);

	/**
	 * Gets the graphics flag for queuing the actor graphics (mirrored or normal)
	 *
	 * @return The graphics flag.
	 */
	DrawFlags getGraphicsFlags();

	/**
	 * Gets the absolute distance for a frame.
	 *
	 * @param direction  The direction.
	 * @param frameIndex Zero-based index of the frame.
	 *
	 * @return The distance for the frame.
	 */
	int32 getAbsoluteDistanceForFrame(ActorDirection direction, uint32 frameIndex) const;

		/**
	 * Gets the distance for a frame.
	 *
	 * @param direction  The direction.
	 * @param frameIndex Zero-based index of the frame.
	 *
	 * @return The distance for the frame.
	 */
	int32 getDistanceForFrame(ActorDirection direction, uint32 frameIndex) const;

	/**
	 * Updates the coordinates depending on the direction.
	 *
	 * @param direction 	 The direction.
	 * @param delta 		 The delta.
	 * @param [in,out] point If non-null, the point.
	 */
	static void updateCoordinatesForDirection(ActorDirection direction, int32 delta, Common::Point *point);

	/**
	 * Get the angle between the two vectors
	 *
	 * @param p1 The first vector.
	 * @param p2 The second vector.
	 *
	 * @return the angle
	 */
	static uint32 angleFromVectors(Common::Point vec1, Common::Point vec2);

	/**
	 * Create a new rect using the point, depending on the actor direction
	 *
	 * @param rect          The rectangle.
	 * @param direction 	The direction.
	 * @param point 		The point.
	 */
	static void rectFromDirection(Common::Rect *rect, ActorDirection direction, Common::Point point);

	/**
	 * Compares the angle between two vectors
	 *
	 * @param vec1 The first vector.
	 * @param vec2 The second vector.
	 *
	 * @return true if ...
	 */
	static bool compareAngles(Common::Point vec1, Common::Point vec2);

	/**
	 * Compares vector vec to two other vectors.
	 *
	 * @param vec1 The first vector.
	 * @param vec2 The second vector.
	 * @param vec  The vector to check
	 *
	 * @return true if vec is between vec1 and vec2.
	 */
	static bool compare(Common::Point vec1, Common::Point vec2, Common::Point vec);

	/**
	 * Compare vectors
	 *
	 * @param vec1 The first vector.
	 * @param vec2 The second vector.
	 * @param vec  The vector.
	 *
	 * @return value depending on whether vec.x is superior or inferior to each vector x coordinate
	 */
	static int32 compareX(Common::Point vec1, Common::Point vec2, Common::Point vec);

	/**
	 * Compare vectors
	 *
	 * @param vec1 The first vector.
	 * @param vec2 The second vector.
	 * @param vec  The vector.
	 *
	 * @return value depending on whether vec.y is superior or inferior to each vector y coordinate
	 */
	static int32 compareY(Common::Point vec1, Common::Point vec2, Common::Point vec);

}; // end of class MainActor

} // end of namespace Asylum

#endif

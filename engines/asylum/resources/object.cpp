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
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.	 See the
 * GNU General Public License for more details.

 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 *
 */

#include "asylum/resources/object.h"

#include "asylum/resources/actor.h"
#include "asylum/resources/special.h"
#include "asylum/resources/worldstats.h"

#include "asylum/system/graphics.h"
#include "asylum/system/screen.h"

#include "asylum/views/scene.h"

#include "asylum/asylum.h"
#include "asylum/respack.h"

namespace Asylum {

Object::Object(AsylumEngine *engine) : x(0), y(0), flags(0), actionType(0),
	_vm(engine), _index(0),
	_id(kObjectNone), _resourceId(kResourceNone), _field_20(0), _frameIndex(0), _frameCount(0),
	_field_2C(0), _field_30(0), _field_34(0), _field_3C(0), _polygonIndex(0), _field_B4(0),
	_tickCount(0), _tickCount2(0), _field_C0(0), _priority(0), _scriptIndex(0), _field_67C(0),
	_soundX(0), _soundY(0), _field_688(0), _soundResourceId(kResourceNone), _field_6A4(kDirectionN)
{
	memset(&_name, 0, sizeof(_name));
	memset(&_gameFlags, 0, sizeof(_gameFlags));
	memset(&_randomResourceIds, 0, sizeof(_randomResourceIds));
}

Object::~Object() {
	// Zero passed pointers
	_vm = NULL;
}

/////////////////////////////////////////////////////////////////////////
// Loading & destroying
/////////////////////////////////////////////////////////////////////////
void Object::load(Common::SeekableReadStream *stream) {
	_id   = (ObjectId)stream->readSint32LE();
	_resourceId = (ResourceId)stream->readSint32LE();
	x	  = stream->readSint32LE();
	y	  = stream->readSint32LE();

	_boundingRect.left	 = (int16)(stream->readSint32LE() & 0xFFFF);
	_boundingRect.top	 = (int16)(stream->readSint32LE() & 0xFFFF);
	_boundingRect.right	 = (int16)(stream->readSint32LE() & 0xFFFF);
	_boundingRect.bottom = (int16)(stream->readSint32LE() & 0xFFFF);

	_field_20   = stream->readSint32LE();
	_frameIndex = stream->readUint32LE();
	_frameCount = stream->readUint32LE();
	_field_2C   = stream->readSint32LE();
	_field_30   = stream->readSint32LE();
	_field_34   = stream->readSint32LE();
	flags       = stream->readUint32LE();
	_field_3C   = stream->readSint32LE();

	stream->read(_name, sizeof(_name));

	_rect.left    = (int16)(stream->readSint32LE());
	_rect.top     = (int16)(stream->readSint32LE());
	_rect.right   = (int16)(stream->readSint32LE());
	_rect.bottom  = (int16)(stream->readSint32LE());

	_polygonIndex = stream->readSint32LE();
	actionType    = stream->readSint32LE();

	for (int i = 0; i < 10; i++)
		_gameFlags[i] = stream->readSint32LE();

	_field_B4	  = stream->readSint32LE();
	_tickCount	  = stream->readUint32LE();
	_tickCount2	  = stream->readUint32LE();
	_field_C0	  = stream->readUint32LE();
	_priority	  = stream->readSint32LE();
	_scriptIndex = stream->readSint32LE();

	for (int i = 0; i < 16; i++) {
		_soundItems[i].resourceId	  = (ResourceId)stream->readSint32LE();
		_soundItems[i].field_4 = stream->readSint32LE();
		_soundItems[i].field_8 = stream->readSint32LE();
		_soundItems[i].field_C = stream->readSint32LE();
	}

	for (int i = 0; i < 50; i++) {
		_frameSoundItems[i].resourceId	= (ResourceId)stream->readSint32LE();
		_frameSoundItems[i].frameIndex = stream->readSint32LE();
		_frameSoundItems[i].index	= stream->readSint32LE();
		_frameSoundItems[i].field_C	= stream->readSint32LE();
		_frameSoundItems[i].field_10 = stream->readSint32LE();
		_frameSoundItems[i].field_14 = stream->readSint32LE();
	}

	_field_67C = stream->readSint32LE();
	_soundX	  = stream->readSint32LE();
	_soundY	  = stream->readSint32LE();
	_field_688 = stream->readSint32LE();

	for (int i = 0; i < 5; i++)
		_randomResourceIds[i] = (ResourceId)stream->readSint32LE();

	_soundResourceId = (ResourceId)stream->readSint32LE();
	_field_6A4       = (ActorDirection)stream->readSint32LE();
}


void Object::disable() {
	flags &= ~kObjectFlagEnabled;
}

void Object::disableAndRemoveFromQueue() {
	disable();

	flags |= kObjectFlag20000;

	getScreen()->deleteGraphicFromQueue(_resourceId);
}

/////////////////////////////////////////////////////////////////////////
// Visibility
//////////////////////////////////////////////////////////////////////////
bool Object::isOnScreen() {
	Common::Rect screenRect = Common::Rect((int16)getWorld()->xLeft, (int16)getWorld()->yTop, (int16)(getWorld()->xLeft + 640), (int16)(getWorld()->yTop + 480));
	Common::Rect objectRect = Common::Rect(_boundingRect);

	objectRect.translate((int16)x, (int16)y);

	return isVisible() && (flags & kObjectFlagEnabled) && screenRect.intersects(objectRect);
}

bool Object::isVisible() const {
	if (flags & kObjectFlagEnabled) {

		// Check each game flag
		for (int32 i = 0; i < 10; i++) {
			int32 flag = _gameFlags[i];
			bool ok = false;

			if (flag <= 0)
				ok = _vm->isGameFlagNotSet((GameFlag)-flag);
			else
				ok = _vm->isGameFlagSet((GameFlag)flag);

			if (!ok)
				return false;
		}

		// All flags were ok, we are done!
		return true;
	}

	return false;
}

void Object::adjustCoordinates(Common::Point *point) {
	if (!point)
		error("[Actor::adjustCoordinates] Invalid point parameter!");

	point->x += x - getWorld()->xLeft;
	point->y += y - getWorld()->yTop;
}

/////////////////////////////////////////////////////////////////////////
// Update
//////////////////////////////////////////////////////////////////////////
void Object::draw() {
	if (LOBYTE(flags) & kObjectFlag4)
		return;

	if (BYTE1(flags) & kObjectFlag40)
		return;

	if (!isOnScreen())
		return;

	// Draw the object
	Common::Point point;
	adjustCoordinates(&point);

	if (_field_67C <= 0 || _field_67C >= 4 || Config.performance <= 1) {
		getScreen()->addGraphicToQueue(_resourceId, _frameIndex, point, (DrawFlags)((flags >> 11) & kDrawFlagMirrorLeftRight), _field_67C - 3, _priority);
	} else {
		getScreen()->addGraphicToQueueCrossfade(_resourceId, _frameIndex, point, getWorld()->backgroundImage, Common::Point(getWorld()->xLeft, getWorld()->yTop), _field_67C - 1);
	}
}

void Object::update() {
	if (_frameCount == 0)
		error("[Object::update] Object has no frame!");

	bool doPlaySounds = false;

	if (_field_3C != 4)
		return;

	if (!isVisible()) {
		updateSoundItems();
		return;
	}

	// Examine flags
	if (flags & kObjectFlag20) {
		if (_vm->getTick() - _tickCount >= (uint32)Common::Rational(1000, _field_B4).toInt()) {
			_frameIndex =((_frameIndex + 1) % _frameCount);
			_tickCount = _vm->getTick();
			doPlaySounds = true;
		}
	} else if (flags & kObjectFlag10) {

		bool isFirstFrame = (_frameIndex == 0);

		if (!_frameIndex) {
			if (_vm->getTick() - _tickCount >= 1000 * _tickCount2) {
				if (_vm->getRandom(_field_C0) == 1) {
					if (_randomResourceIds[0]) {
						_resourceId = getRandomResourceId();
						_frameCount  = GraphicResource::getFrameCount(_vm, _resourceId);
					}
					_frameIndex++;
				}
				_tickCount = _vm->getTick();
				doPlaySounds = true;
			}

			isFirstFrame = (_frameIndex == 0);
		}

		if (!isFirstFrame) {
			if (_vm->getTick() - _tickCount >= (uint32)Common::Rational(1000, _field_B4).toInt()) {
				_frameIndex =((_frameIndex + 1) % _frameCount);
				_tickCount = _vm->getTick();
				doPlaySounds = true;
			}
		}
	} else if (BYTE1(flags) & kObjectFlag8) {
		if (_vm->getTick() - _tickCount >= 1000 * _tickCount2) {
			if (_vm->getRandom(_field_C0) == 1)
				_frameIndex =((_frameIndex + 1) % _frameCount);

			_tickCount = _vm->getTick();
			doPlaySounds = true;
		}
	} else if (flags & kObjectFlag8) {

		if (_vm->getTick() - _tickCount >= (uint32)Common::Rational(1000, _field_B4).toInt()) {

			++_frameIndex;

			if (_frameIndex < _frameCount - 1) {
				if (_field_688 == 1) {
					Common::Rect frameRect = GraphicResource::getFrameRect(_vm, _resourceId, _frameIndex);
					getSharedData()->point.x = x + frameRect.left + Common::Rational(frameRect.width(), 2).toInt();
					getSharedData()->point.y = y + frameRect.top  + Common::Rational(frameRect.height(), 2).toInt();
				}
			} else {
				flags &= ~kObjectFlag8;
				if (_field_688 == 1) {
					getSharedData()->point.x = -1;
					getSharedData()->point.y = -1;
				}
			}
			_tickCount = _vm->getTick();
			doPlaySounds = true;
		}
	} else if (!(BYTE1(flags) & kObjectFlag6)) {

		if ((flags & kObjectFlag10000) && (_vm->getTick() - _tickCount >= (uint32)Common::Rational(1000, _field_B4).toInt())) {

			++_frameIndex;

			if (_frameIndex == 0) {
				flags &= ~kObjectFlag10000;
				if (_field_688 == 1) {
					getSharedData()->point.x = -1;
					getSharedData()->point.y = -1;
				}
			} else if (_field_688 == 1) {
				Common::Rect frameRect = GraphicResource::getFrameRect(_vm, _resourceId, _frameIndex);
				getSharedData()->point.x = x + frameRect.left + Common::Rational(frameRect.width(), 2).toInt();
				getSharedData()->point.y = y + frameRect.top  + Common::Rational(frameRect.height(), 2).toInt();
			}

			_tickCount = _vm->getTick();
			doPlaySounds = true;
		}
	} else if (_vm->getTick() - _tickCount >= (uint32)Common::Rational(1000, _field_B4).toInt()) {
		if (BYTE1(flags) & kObjectFlag2) {
			if (_frameIndex == _frameCount - 1) {
				_frameIndex--;
				flags = (BYTE1(flags) & 0xFD) | kObjectFlag4;
			} else {
				_frameIndex++;
			}
		} else if (BYTE1(flags) & kObjectFlag4) {
			if (_frameIndex) {
				_frameIndex--;
			} else {
				_frameIndex++;
				flags = (BYTE1(flags) & 0xFB) | kObjectFlag2;
			}
		}
	}

	if (flags & kObjectFlag40000) {
		if (_frameIndex == _frameCount - 1) {
			if (_field_B4 <= 15) {
				_field_B4 -= 2;

				if (_field_B4 < 0)
					_field_B4 = 0;
			} else {
				_field_B4 = 15;
			}

			if (!_field_B4)
				flags &= ~kObjectFlag10E38;
		}
	}

	if (doPlaySounds)
		playSounds();

	getSpecial()->run(this, -1);
}

void Object::setNextFrame(int32 targetFlags) {
	flags |= targetFlags | kObjectFlagEnabled;

	if (flags & kObjectFlag10000)
		_frameIndex = _frameCount - 1;
	else
		_frameIndex = 0;
}

/////////////////////////////////////////////////////////////////////////
// Misc
/////////////////////////////////////////////////////////////////////////
void Object::playSounds() {
	Common::Point point;

	if (_soundX || _soundY) {
		point.x = _soundX;
		point.y = _soundY;
	} else {
		if (LOBYTE(flags) & kObjectFlag4) {
			// Get object resource
			ResourceEntry *resource = getResource()->get(_resourceId);

			point.x = x + Common::Rational(resource->getData(1), 2).toInt();
			point.y = y + Common::Rational(resource->getData(0), 2).toInt();
		} else {
			Common::Rect rect = GraphicResource::getFrameRect(_vm, _resourceId, _frameIndex);

			point.x = x + (rect.width() * 2);
			point.y = x + (rect.height() * 2);
		}
	}

	for (int i = 0; i < ARRAYSIZE(_soundItems); i++) {
		SoundItem *item = &_soundItems[i];

		if (item->resourceId == kResourceNone)
			continue;

		if (item->field_4 && !getSound()->isPlaying(item->resourceId)) {
			int32 volume = Config.sfxVolume + getSound()->calculateVolumeAdjustement(point, item->field_8, item->field_C);

			if (volume > -5000)
				getSound()->playSound(item->resourceId, true, volume, getSound()->calculatePanningAtPoint(point));
		}

		if (getSound()->isPlaying(item->resourceId)) {
			int32 volume = Config.sfxVolume + getSound()->calculateVolumeAdjustement(point, item->field_8, item->field_C);

			if (volume > -5000) {
				if (volume > 0)
					volume = 0;

				getSound()->setPanning(item->resourceId, getSound()->calculatePanningAtPoint(point));
				getSound()->setVolume(item->resourceId, volume);
			} else {
				getSound()->stop(item->resourceId);
			}
		}
	}

	setVolume();
}

void Object::updateSoundItems() {
	for (int32 i = 0; i < ARRAYSIZE(_soundItems); i++) {

		SoundItem *item = &_soundItems[i];

		if (getSound()->isPlaying(item->resourceId)) {
			if (item->field_4) {
				getSound()->stop(item->resourceId);
				item->resourceId = kResourceNone;
				item->field_4 = 0;
			}
		}
	}

	stopSound();
}

void Object::stopSound() {
	if (getSound()->isPlaying(_soundResourceId))
		getSound()->stop(_soundResourceId);
}

void Object::stopAllSounds() {
	for (int i = 0; i < ARRAYSIZE(_soundItems); i++)
		if (_soundItems[i].resourceId) {
			getSound()->stop(_soundItems[i].resourceId);
			_soundItems[i].resourceId = kResourceNone;
		}
}

void Object::setVolume() {
	if (!_soundResourceId || !getSound()->isPlaying(_soundResourceId))
		return;

	Common::Rect frameRect = GraphicResource::getFrameRect(_vm, _resourceId, _frameIndex);

	// Compute volume
	Common::Point coords(Common::Rational(frameRect.width(), 2).toInt() + x, Common::Rational(frameRect.height(), 2).toInt() + y);
	int32 volume = Config.voiceVolume + getSound()->calculateVolumeAdjustement(coords, _field_6A4, 0);
	if (volume < -10000)
		volume = -10000;

	getSound()->setVolume(_soundResourceId, volume);
}

ResourceId Object::getRandomResourceId() {
	// Initialize random resource id array
	ResourceId shuffle[5];
	memset(&shuffle, 0, sizeof(shuffle));
	uint32 count = 0;

	for (int32 i = 0; i < 5; i++) {
		if (_randomResourceIds[i]) {
			shuffle[count] = _randomResourceIds[i];
			count++;
		}
	}

	if (count == 0)
		error("[Object::getRandomId] Could not get a random resource id!");

	ResourceId id = shuffle[_vm->getRandom(count - 1)];
	if (id == kResourceNone)
		error("[Object::getRandomId] Got an empty resource id!");

	return id;
}

bool Object::checkFlags() const {
	return (flags & kObjectFlagEnabled) && (flags & kObjectFlag8 || flags & kObjectFlag10000);
}

Common::String Object::toString(bool shortString) {
	Common::String output;

	output += Common::String::format("Object %d: %s\n", _id, _name);

	if (!shortString) {
		output += Common::String::format("    resourceId:      %u (0x%X) - (pack %d - index %d)\n", _resourceId, _resourceId, RESOURCE_PACK(_resourceId), RESOURCE_INDEX(_resourceId));
		output += Common::String::format("    x:               %d\n", x);
		output += Common::String::format("    y:               %d\n", y);
		output += Common::String::format("    flags:           %d\n", flags);
		output += Common::String::format("    actionType:      %d\n", actionType);
		output += Common::String::format("    boundingRect:    top[%d], left[%d], right[%d], bottom[%d]\n", _boundingRect.top, _boundingRect.left, _boundingRect.right, _boundingRect.bottom);
		output += Common::String::format("    field_20:        %d\n", _field_20);
		output += Common::String::format("    frameIndex:      %u\n", _frameIndex);
		output += Common::String::format("    frameCount:      %u\n", _frameCount);
		output += Common::String::format("    field_2C:        %d\n", _field_2C);
		output += Common::String::format("    field_30:        %d\n", _field_30);
		output += Common::String::format("    field_34:        %d\n", _field_34);
		output += Common::String::format("    field_3C:        %d\n", _field_3C);
		output += Common::String::format("    rect:            top[%d], left[%d], right[%d], bottom[%d]\n", _rect.top, _rect.left, _rect.right, _rect.bottom);
		output += Common::String::format("    polygonIndex:    %d\n", _polygonIndex);
		output += Common::String::format("    field_B4:        %d\n", _field_B4);
		output += Common::String::format("    tickCount:       %d\n", _tickCount);
		output += Common::String::format("    tickCount2:      %d\n", _tickCount2);
		output += Common::String::format("    field_C0:        %d\n", _field_C0);
		output += Common::String::format("    priority:        %d\n", _priority);
		output += Common::String::format("    scriptIndex:   %d\n", _scriptIndex);
		output += Common::String::format("    field_67C:       %d\n", _field_67C);
		output += Common::String::format("    soundX:          %d\n", _soundX);
		output += Common::String::format("    soundY:          %d\n", _soundY);
		output += Common::String::format("    field_688:       %d\n", _field_688);
		output += Common::String::format("    soundResourceId: %d\n", _soundResourceId);
		output += Common::String::format("    field_6A4:       %d\n", _field_6A4);

	}

	return output;
}

} // end of namespace Asylum

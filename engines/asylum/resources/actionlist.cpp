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
 * $URL$
 * $Id$
 *
 */

#include "asylum/resources/actionlist.h"

#include "asylum/resources/actor.h"
#include "asylum/resources/encounters.h"
#include "asylum/resources/object.h"
#include "asylum/resources/worldstats.h"

#include "asylum/system/config.h"
#include "asylum/system/cursor.h"
#include "asylum/system/graphics.h"
#include "asylum/system/screen.h"
#include "asylum/system/speech.h"

#include "asylum/views/menu.h"
#include "asylum/views/scene.h"

#include "asylum/asylum.h"
#include "asylum/staticres.h"

#include "common/rational.h"

namespace Asylum {

ActionList::ActionList(AsylumEngine *engine) : _vm(engine) {
	// Build list of opcodes
	ADD_OPCODE(Return);
	ADD_OPCODE(SetGameFlag);
	ADD_OPCODE(ClearGameFlag);
	ADD_OPCODE(ToggleGameFlag);
	ADD_OPCODE(JumpIfGameFlag);
	ADD_OPCODE(HideCursor);
	ADD_OPCODE(ShowCursor);
	ADD_OPCODE(PlayAnimation);
	ADD_OPCODE(MoveScenePosition);
	ADD_OPCODE(HideActor);
	ADD_OPCODE(ShowActor);
	ADD_OPCODE(SetActorPosition);
	ADD_OPCODE(SetSceneMotionStatus);
	ADD_OPCODE(DisableActor);
	ADD_OPCODE(EnableActor);
	ADD_OPCODE(EnableObjects);
	ADD_OPCODE(Return);
	ADD_OPCODE(RemoveObject);
	ADD_OPCODE(JumpActorSpeech);
	ADD_OPCODE(JumpAndSetDirection);
	ADD_OPCODE(JumpIfActorCoordinates);
	ADD_OPCODE(Nop);
	ADD_OPCODE(ResetAnimation);
	ADD_OPCODE(DisableObject);
	ADD_OPCODE(JumpIfSoundPlayingAndPlaySound);
	ADD_OPCODE(JumpIfActionFind);
	ADD_OPCODE(SetActionFind);
	ADD_OPCODE(ClearActionFind);
	ADD_OPCODE(JumpIfActionGrab);
	ADD_OPCODE(SetActionGrab);
	ADD_OPCODE(ClearActionGrab);
	ADD_OPCODE(JumpIfActionTalk);
	ADD_OPCODE(SetActionTalk);
	ADD_OPCODE(ClearActionTalk);
	ADD_OPCODE(_unk22);
	ADD_OPCODE(_unk23);
	ADD_OPCODE(_unk24);
	ADD_OPCODE(RunEncounter);
	ADD_OPCODE(JumpIfAction16);
	ADD_OPCODE(SetAction16);
	ADD_OPCODE(ClearAction16);
	ADD_OPCODE(SetActorField638);
	ADD_OPCODE(JumpIfActorField638);
	ADD_OPCODE(ChangeScene);
	ADD_OPCODE(_unk2C_ActorSub);
	ADD_OPCODE(PlayMovie);
	ADD_OPCODE(StopAllObjectsSounds);
	ADD_OPCODE(StopProcessing);
	ADD_OPCODE(ResumeProcessing);
	ADD_OPCODE(ResetSceneRect);
	ADD_OPCODE(ChangeMusicById);
	ADD_OPCODE(StopMusic);
	ADD_OPCODE(_unk34_Status);
	ADD_OPCODE(SetVolume);
	ADD_OPCODE(Jump);
	ADD_OPCODE(RunBlowUpPuzzle);
	ADD_OPCODE(JumpIfAction8);
	ADD_OPCODE(SetAction8);
	ADD_OPCODE(ClearAction8);
	ADD_OPCODE(_unk3B_PALETTE_MOD);
	ADD_OPCODE(IncrementParam2);
	ADD_OPCODE(WaitUntilFramePlayed);
	ADD_OPCODE(UpdateWideScreen);
	ADD_OPCODE(JumpIfActor);
	ADD_OPCODE(PlaySpeechScene);
	ADD_OPCODE(PlaySpeech);
	ADD_OPCODE(PlaySpeechScene2);
	ADD_OPCODE(MoveScenePositionFromActor);
	ADD_OPCODE(PaletteFade);
	ADD_OPCODE(StartPaletteFadeThread);
	ADD_OPCODE(_unk46);
	ADD_OPCODE(ActorFaceObject);
	ADD_OPCODE(_unk48_MATTE_01);
	ADD_OPCODE(_unk49_MATTE_90);
	ADD_OPCODE(JumpIfSoundPlaying);
	ADD_OPCODE(ChangePlayerActorIndex);
	ADD_OPCODE(ChangeActorStatus);
	ADD_OPCODE(StopSound);
	ADD_OPCODE(JumpRandom);
	ADD_OPCODE(ClearScreen);
	ADD_OPCODE(Quit);
	ADD_OPCODE(JumpObjectFrame);
	ADD_OPCODE(DeleteGraphics);
	ADD_OPCODE(DeleteGraphics);
	ADD_OPCODE(_unk54_SET_ACTIONLIST_6EC);
	ADD_OPCODE(_unk55);
	ADD_OPCODE(_unk56);
	ADD_OPCODE(SetResourcePalette);
	ADD_OPCODE(SetObjectFrameIdxFlaged);
	ADD_OPCODE(_unk59);
	ADD_OPCODE(_unk5A);
	ADD_OPCODE(_unk5B);
	ADD_OPCODE(QueueScript);
	ADD_OPCODE(_unk5D);
	ADD_OPCODE(ClearActorFields);
	ADD_OPCODE(SetObjectLastFrameIdx);
	ADD_OPCODE(_unk60_SET_OR_CLR_ACTIONAREA_FLAG);
	ADD_OPCODE(_unk61);
	ADD_OPCODE(ShowOptionsScreen);
	ADD_OPCODE(_unk63);

	reset();
}

ActionList::~ActionList() {
	for (int i = 0; i < (int)_opcodes.size(); i++)
		delete _opcodes[i];

	_scripts.clear();
	_queue.clear();

	// Zero-out passed pointers
	_vm = NULL;
}

void ActionList::load(Common::SeekableReadStream *stream) {
	stream->readSint32LE();  // size
	int32 numEntries = stream->readSint32LE();

	for (int32 a = 0; a < numEntries; a++) {
		Script script;
		memset(&script, 0, sizeof(Script));

		for (int32 c = 0; c < MAX_ACTION_COMMANDS; c++) {
			ScriptEntry command;
			memset(&command, 0, sizeof(ScriptEntry));

			command.numLines = stream->readSint32LE();
			command.opcode   = stream->readSint32LE();
			command.param1   = stream->readSint32LE();
			command.param2   = stream->readSint32LE();
			command.param3   = stream->readSint32LE();
			command.param4   = stream->readSint32LE();
			command.param5   = stream->readSint32LE();
			command.param6   = stream->readSint32LE();
			command.param7   = stream->readSint32LE();
			command.param8   = stream->readSint32LE();
			command.param9   = stream->readSint32LE();

			script.commands[c] = command;
		}

		script.field_1BAC = stream->readSint32LE();
		script.field_1BB0 = stream->readSint32LE();
		script.counter    = stream->readSint32LE();

		_scripts.push_back(script);
	}
}

void ActionList::reset() {
	// Reset script queue
	resetQueue();

	_skipProcessing    = false;
	_currentLine       = 0;
	_currentLoops      = 0;
	_currentScript     = NULL;
	_delayedSceneIndex = kResourcePackInvalid;
	_delayedVideoIndex = -1;
	_done = false;
	_exit = false;
	_lineIncrement = 0;
	_waitCycle = false;
}

void ActionList::resetQueue() {
	_queue.clear();
}

void ActionList::queueScript(int scriptIndex, ActorIndex actorIndex) {
	// When the skipProcessing flag is set, do not queue any more scripts
	if (_skipProcessing)
		return;

	ScriptQueueEntry entry;
	entry.scriptIndex = scriptIndex;
	entry.actorIndex  = actorIndex;

	// If there's currently no script for the processor to run,
	// assign it directly and skip the stack push. If however the
	// current script is assigned, push the script to the stack
	if (_currentScript)
		_queue.push(entry);
	else {
		_currentQueueEntry = entry;
		_currentScript     = &_scripts[entry.scriptIndex];
	}
}

bool ActionList::process() {
	_done          = false;
	_exit          = false;
	_waitCycle     = false;
	_lineIncrement = 1;

	_vm->setGameFlag(kGameFlagScriptProcessing);

	if (_currentScript) {
		while (!_done && !_waitCycle) {
			_lineIncrement = 0; //Reset line increment value

			ScriptEntry *cmd = &_currentScript->commands[_currentLine];

			int32 opcode = cmd->opcode;

			debugC(kDebugLevelScripts, "[0x%02X] %s (%d, %d, %d, %d, %d, %d, %d, %d, %d)",
			       opcode, _opcodes[opcode]->name,
			       cmd->param1, cmd->param2, cmd->param3, cmd->param4, cmd->param5,
			       cmd->param6, cmd->param7, cmd->param8, cmd->param9);

			// Execute opcode
			(*_opcodes[opcode]->func)(cmd);

			if (_exit)
				return true;

			if (!_lineIncrement)
				_currentLine ++;
		}

		if (_done) {
			_currentLine  = 0;

			if (!_queue.empty()) {
				_currentQueueEntry = _queue.pop();
				_currentScript     = &_scripts[_currentQueueEntry.scriptIndex];
			} else {
				_currentScript = NULL;
			}
		}
	}

	_vm->clearGameFlag(kGameFlagScriptProcessing);

	return false;
}

//////////////////////////////////////////////////////////////////////////
// Opcode Functions
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// Opcode 0x00
IMPLEMENT_OPCODE(Return) {
	_done          = true;
	_lineIncrement = 0;
}

//////////////////////////////////////////////////////////////////////////
// Opcode 0x01
IMPLEMENT_OPCODE(SetGameFlag) {
	GameFlag flagNum = (GameFlag)cmd->param1;

	if (flagNum >= 0)
		_vm->setGameFlag(flagNum);
}

//////////////////////////////////////////////////////////////////////////
// Opcode 0x02
IMPLEMENT_OPCODE(ClearGameFlag) {
	GameFlag flagNum = (GameFlag)cmd->param1;

	if (flagNum >= 0)
		_vm->clearGameFlag(flagNum);
}

//////////////////////////////////////////////////////////////////////////
// Opcode 0x03
IMPLEMENT_OPCODE(ToggleGameFlag) {
	GameFlag flagNum = (GameFlag)cmd->param1;

	if (flagNum >= 0)
		_vm->toggleGameFlag(flagNum);
}

//////////////////////////////////////////////////////////////////////////
// Opcode 0x04
IMPLEMENT_OPCODE(JumpIfGameFlag) {
	if (cmd->param1 < 0)
		return;

	bool doJump = (cmd->param2) ? _vm->isGameFlagSet((GameFlag)cmd->param1) : _vm->isGameFlagNotSet((GameFlag)cmd->param1);
	if (!doJump)
		return;

	setNextLine(cmd->param3);
}

//////////////////////////////////////////////////////////////////////////
// Opcode 0x05
IMPLEMENT_OPCODE(HideCursor) {
	getCursor()->hide();
}

//////////////////////////////////////////////////////////////////////////
// Opcode 0x06
IMPLEMENT_OPCODE(ShowCursor) {
	getCursor()->show();

	_vm->clearFlag(kFlagType1);
}

//////////////////////////////////////////////////////////////////////////
// Opcode 0x07
IMPLEMENT_OPCODE(PlayAnimation) {
	Object *object = getWorld()->getObjectById((ObjectId)cmd->param1);

	if (cmd->param2 == 2) {
		if (object->checkFlags())
			_lineIncrement = 1;
		else
			cmd->param2 = 1;

		return;
	}

	// Update flags
	if (cmd->param4) {
		object->flags &= ~kObjectFlag10E38;
        object->flags |= kObjectFlag20;
	} else if (cmd->param3) {
		object->flags &= ~kObjectFlag10E38;
		object->flags |= kObjectFlag10000;
	} else if (object->flags & kObjectFlag10000) {
		object->flags |= kObjectFlag8;
		object->flags &= ~kObjectFlag10000;
	} else if (!(object->flags & kObjectFlag10E38)) {
		object->flags |= kObjectFlag8;
	}

	object->setNextFrame(object->flags);

	if (object->getField688() == 1) {
		if (object->flags & kObjectFlag4) {
			getScene()->setGlobalX(object->x);
			getScene()->setGlobalY(object->y);
		} else {
			GraphicResource *res = new GraphicResource(_vm, object->getResourceId());
			GraphicFrame *frame = res->getFrame(object->getFrameIndex());

			getScene()->setGlobalX(frame->x + (frame->getWidth() >> 1) + object->x);
			getScene()->setGlobalY(frame->y + (frame->getHeight() >> 1) + object->y);

			delete res;
		}
	}

	if (cmd->param2) {
		cmd->param2 = 2;
		_lineIncrement = 1;
	}
}

//////////////////////////////////////////////////////////////////////////
// Opcode 0x08
IMPLEMENT_OPCODE(MoveScenePosition) {
	if (cmd->param3 < 1) {
		getWorld()->xLeft = cmd->param1;
		getWorld()->yTop  = cmd->param2;
		getWorld()->motionStatus = 3;

	} else if (!cmd->param4) {
		getWorld()->motionStatus = 5;

		getScene()->updateSceneCoordinates(cmd->param1,
			                           cmd->param2,
		                               cmd->param3);

	} else if (cmd->param5) {
		if (getWorld()->motionStatus == 2)
			_lineIncrement = 1;
		else
			cmd->param5 = 0;
	} else {
		cmd->param5 = 1;
		getWorld()->motionStatus = 2;

		getScene()->updateSceneCoordinates(cmd->param1,
		                               cmd->param2,
		                               cmd->param3,
		                               true);

		_lineIncrement = 1;
	}
}

//////////////////////////////////////////////////////////////////////////
// Opcode 0x09
IMPLEMENT_OPCODE(HideActor) {
	Actor *actor = getScene()->getActor(cmd->param1);

	actor->hide();
	actor->updateDirection();
}

//////////////////////////////////////////////////////////////////////////
// Opcode 0x0A
IMPLEMENT_OPCODE(ShowActor) {
	Actor *actor = getScene()->getActor(cmd->param1);

	actor->show();
	actor->updateDirection();
	actor->setLastScreenUpdate(_vm->getTick());
}

//////////////////////////////////////////////////////////////////////////
// Opcode 0x0B
IMPLEMENT_OPCODE(SetActorPosition) {
	Actor *actor = getScene()->getActor(cmd->param1);

	actor->setPosition(cmd->param2, cmd->param3, cmd->param4, cmd->param5);
}

//////////////////////////////////////////////////////////////////////////
// Opcode 0x0C
IMPLEMENT_OPCODE(SetSceneMotionStatus) {
	getWorld()->motionStatus = cmd->param1;
}

//////////////////////////////////////////////////////////////////////////
// Opcode 0x0D
IMPLEMENT_OPCODE(DisableActor) {
	Actor *actor = getScene()->getActor(cmd->param1);

	actor->updateStatus(kActorStatusDisabled);
}

//////////////////////////////////////////////////////////////////////////
// Opcode 0x0E
IMPLEMENT_OPCODE(EnableActor) {
	Actor *actor = getScene()->getActor(cmd->param1);

	if (actor->getStatus() == kActorStatusDisabled)
		actor->updateStatus(kActorStatusEnabled);
}

//////////////////////////////////////////////////////////////////////////
// Opcode 0x0F
IMPLEMENT_OPCODE(EnableObjects) {
	Object *object = getWorld()->getObjectById((ObjectId)cmd->param1);

	if (!_currentScript->counter && getWorld()->chapter != 13)
		_vm->sound()->playSound(cmd->param3 ? MAKE_RESOURCE(kResourcePackSound, 6) : MAKE_RESOURCE(kResourcePackSound, 1));

	if (_currentScript->counter >= (3 * cmd->param2 - 1)) {
		_currentScript->counter = 0;
		object->setField67C(0);
		enableObject(cmd, kObjectEnableType2);
	} else {
		++_currentScript->counter;
		if (cmd->param3) {
			object->setField67C(3 - _currentScript->counter / cmd->param2);
			enableObject(cmd, kObjectEnableType1);
		} else {
			object->setField67C(_currentScript->counter / cmd->param2 + 1);
			enableObject(cmd, kObjectEnableType0);
		}

		_lineIncrement = 1;
	}
}

//////////////////////////////////////////////////////////////////////////
// Opcode 0x10 : Identical to opcode 0x00

//////////////////////////////////////////////////////////////////////////
// Opcode 0x11
IMPLEMENT_OPCODE(RemoveObject) {
	if (!cmd->param1)
		return;

	Object *object = getWorld()->getObjectById((ObjectId)cmd->param1);

	object->disableAndRemoveFromQueue();
}

//////////////////////////////////////////////////////////////////////////
// Opcode 0x12
IMPLEMENT_OPCODE(JumpActorSpeech) {
	Actor *actor = getScene()->getActor(cmd->param1);

	if (actor->process(cmd->param2, cmd->param3))
		return;

	_currentLine = cmd->param4;

	if (cmd->param5)
		getSpeech()->playIndexed(1);
}

//////////////////////////////////////////////////////////////////////////
// Opcode 0x13
IMPLEMENT_OPCODE(JumpAndSetDirection) {
	Actor *actor = getScene()->getActor(cmd->param1);

	if (actor->getStatus() != kActorStatus2 && actor->getStatus() != kActorStatus13) {
		if (cmd->param5 != 2) {

			if (cmd->param2 == -1 || cmd->param3 == -1) {
				actor->updateFromDirection(cmd->param4);
			} else if ((actor->x1 + actor->x2) == cmd->param2 && (actor->y1 + actor->y2) == cmd->param3) {
				actor->updateFromDirection(cmd->param4);
			} else {
				actor->processStatus(cmd->param2, cmd->param3, cmd->param4);

				if (cmd->param5 == 1) {
					cmd->param5 = 2;
					_lineIncrement = 1;
				}
			}
		} else {
			cmd->param5 = 1;
			_lineIncrement = 0;

			if ((actor->x1 + actor->x2) == cmd->param2 && (actor->y1 + actor->y2) == cmd->param3)
				actor->updateFromDirection(cmd->param4);
		}
	} else {
		if (cmd->param5 == 2)
			_lineIncrement = 1;
	}
}

//////////////////////////////////////////////////////////////////////////
// Opcode 0x14
IMPLEMENT_OPCODE(JumpIfActorCoordinates) {
	Actor *actor = getScene()->getActor(cmd->param1);

	if ((actor->x1 + actor->x2) != cmd->param2 || (actor->y1 + actor->y2) != cmd->param3)
		_lineIncrement = cmd->param4;
}

//////////////////////////////////////////////////////////////////////////
// Opcode 0x15
IMPLEMENT_OPCODE(Nop) {
	// Nothing to do
}

//////////////////////////////////////////////////////////////////////////
// Opcode 0x16
IMPLEMENT_OPCODE(ResetAnimation) {
	Object *object = getWorld()->getObjectById((ObjectId)cmd->param1);

	if (object->flags & kObjectFlag10000)
		object->setFrameIndex(object->getFrameCount() - 1);
	else
		object->setFrameIndex(0);
}

//////////////////////////////////////////////////////////////////////////
// Opcode 0x17
IMPLEMENT_OPCODE(DisableObject) {
	Object *object = getWorld()->getObjectById((ObjectId)cmd->param1);

	object->disable();
}

//////////////////////////////////////////////////////////////////////////
// Opcode 0x18
IMPLEMENT_OPCODE(JumpIfSoundPlayingAndPlaySound) {
	if (cmd->param2 == 2) {
		if (getSound()->isPlaying((ResourceId)cmd->param1))
			_lineIncrement = 1;
		else
			cmd->param2 = 1;
	} else if (!_vm->sound()->isPlaying((ResourceId)cmd->param1)) {
		int32 vol = getSound()->getAdjustedVolume(abs(Config.sfxVolume));
		getSound()->playSound((ResourceId)cmd->param1, cmd->param4, -((abs(cmd->param3) + vol) * (abs(cmd->param3) + vol)), 0);

		if (cmd->param2 == 1) {
			cmd->param2 = 2;
			_lineIncrement= 1;
		}
	}
}

//////////////////////////////////////////////////////////////////////////
// Opcode 0x19
IMPLEMENT_OPCODE(JumpIfActionFind) {
	jumpIfActionFlag(cmd, kActionTypeFind);
}

//////////////////////////////////////////////////////////////////////////
// Opcode 0x1A
IMPLEMENT_OPCODE(SetActionFind) {
	setActionFlag(cmd, kActionTypeFind);
}

//////////////////////////////////////////////////////////////////////////
// Opcode 0x1B
IMPLEMENT_OPCODE(ClearActionFind) {
	clearActionFlag(cmd, kActionTypeFind);
}

//////////////////////////////////////////////////////////////////////////
// Opcode 0x1C
IMPLEMENT_OPCODE(JumpIfActionGrab) {
	jumpIfActionFlag(cmd, kActionTypeGrab);
}

//////////////////////////////////////////////////////////////////////////
// Opcode 0x1D
IMPLEMENT_OPCODE(SetActionGrab) {
	setActionFlag(cmd, kActionTypeGrab);
}

//////////////////////////////////////////////////////////////////////////
// Opcode 0x1E
IMPLEMENT_OPCODE(ClearActionGrab) {
	clearActionFlag(cmd, kActionTypeGrab);
}

//////////////////////////////////////////////////////////////////////////
// Opcode 0x1F
IMPLEMENT_OPCODE(JumpIfActionTalk) {
	jumpIfActionFlag(cmd, kActionTypeTalk);
}

//////////////////////////////////////////////////////////////////////////
// Opcode 0x20
IMPLEMENT_OPCODE(SetActionTalk) {
	setActionFlag(cmd, kActionTypeTalk);
}

//////////////////////////////////////////////////////////////////////////
// Opcode 0x21
IMPLEMENT_OPCODE(ClearActionTalk) {
	clearActionFlag(cmd, kActionTypeTalk);
}

//////////////////////////////////////////////////////////////////////////
// Opcode 0x22
IMPLEMENT_OPCODE(_unk22) {
	Actor *actor = getScene()->getActor(cmd->param3 ? cmd->param3 : _currentQueueEntry.actorIndex);

	actor->process_41BC00(cmd->param1, cmd->param2);
}

//////////////////////////////////////////////////////////////////////////
// Opcode 0x23
IMPLEMENT_OPCODE(_unk23) {
	Actor *actor = getScene()->getActor(cmd->param3 ? cmd->param3 : _currentQueueEntry.actorIndex);

	actor->process_41BCC0(cmd->param1, cmd->param2);
}

//////////////////////////////////////////////////////////////////////////
// Opcode 0x24
IMPLEMENT_OPCODE(_unk24) {
	Actor *actor = getScene()->getActor(cmd->param4 ? cmd->param4 : _currentQueueEntry.actorIndex);

	actor->process_41BDB0(cmd->param1, cmd->param3);
}

//////////////////////////////////////////////////////////////////////////
// Opcode 0x25
IMPLEMENT_OPCODE(RunEncounter) {
	Encounter *encounter = _vm->encounter();

	encounter->setFlag(kEncounterFlag5, cmd->param5);

	if (cmd->param6) {
		if (encounter->getFlag(kEncounterFlag2))
			_lineIncrement = 1;
		else
			cmd->param6 = 0;
	} else {
		encounter->run(cmd->param1, cmd->param2, cmd->param3, cmd->param4);

		cmd->param6 = 2;
		_lineIncrement = 1;
	}
}

//////////////////////////////////////////////////////////////////////////
// Opcode 0x26
IMPLEMENT_OPCODE(JumpIfAction16) {
	jumpIfActionFlag(cmd, kActionType16);
}

//////////////////////////////////////////////////////////////////////////
// Opcode 0x27
IMPLEMENT_OPCODE(SetAction16) {
	setActionFlag(cmd, kActionType16);
}

//////////////////////////////////////////////////////////////////////////
// Opcode 0x28
IMPLEMENT_OPCODE(ClearAction16) {
	clearActionFlag(cmd, kActionType16);
}

//////////////////////////////////////////////////////////////////////////
// Opcode 0x29
IMPLEMENT_OPCODE(SetActorField638) {
	Actor *actor = getScene()->getActor(cmd->param1);

	actor->setField638(cmd->param2);
}

//////////////////////////////////////////////////////////////////////////
// Opcode 0x2A
IMPLEMENT_OPCODE(JumpIfActorField638) {
	Actor *actor = getScene()->getActor(cmd->param1);

	if (actor->getField638())
		_currentLine = cmd->param3;
}

//////////////////////////////////////////////////////////////////////////
// Opcode 0x2B
IMPLEMENT_OPCODE(ChangeScene) {
	getScene()->getActor(0)->updateStatus(kActorStatusDisabled);
	resetQueue();

	// Fade screen to black
	getScreen()->paletteFade(0, 75, 8);
	getScreen()->clearScreen();

	// Stop all sounds & music
	_vm->sound()->stopAllSounds(true);

	// Change the scene number
	_delayedSceneIndex = (ResourcePackId)(cmd->param1 + 4);

	_exit = true;
}

//////////////////////////////////////////////////////////////////////////
// Opcode 0x2C
IMPLEMENT_OPCODE(_unk2C_ActorSub) {
	Actor *player = getScene()->getActor();
	Actor *actor = getScene()->getActor(_currentQueueEntry.actorIndex);
	Common::Point playerPoint(player->x1 + player->x2, player->y1 + player->y2);
	ActorDirection direction = (cmd->param2 == 8) ? player->getDirection() : cmd->param2;

	if (cmd->param2 == 8)
		cmd->param2 = player->getDirection();

	if (cmd->param3 == 2) {
		switch (actor->getStatus()) {
		default:
			_lineIncrement = 1;
			return;

		case kActorStatus7:
			actor->updateStatus(kActorStatusEnabled);
			break;

		case kActorStatus16:
			// We want to continue processing and not go into the default case
			break;

		case kActorStatus20:
			actor->updateStatus(kActorStatus14);
		}

		cmd->param3 = 0;

		if (cmd->param1 == 2) {
			Common::Point point(playerPoint);

			int32 index = (player->getDirection() + 4) % 8;

			if (player->process_408B20(&point, index, 3, false)) {

				point.x += 3 * deltaPointsArray[index].x;
				point.x += 3 * deltaPointsArray[index].y;

				player->setPosition(point.x, point.y, actor->getDirection(), 0);
			}
		}

	} else if (cmd->param1 != 2 || player->process_408B20(&playerPoint, (player->getDirection() + 4) % 8, 3, false)) {
		ResourceId id = kResourceNone;
		if (direction >= 5)
			id = actor->getResourcesId(5 * cmd->param1 - direction + 38);
		else
			id = actor->getResourcesId(5 * cmd->param1 + direction + 30);

		GraphicResource *res = new GraphicResource(_vm, id);
		actor->setResourceId(id);
		actor->setFrameCount(res->getFrameCount());
		actor->setFrameIndex(0);
		actor->setDirection(direction);
		actor->updateStatus(actor->getStatus() <= kActorStatus11 ? kActorStatus3 : kActorStatus19);
		delete res;

		cmd->param3 = 2;
		_lineIncrement = 1;
	}
}

//////////////////////////////////////////////////////////////////////////
// Opcode 0x2D
IMPLEMENT_OPCODE(PlayMovie) {
	if (getScene()->matteBarHeight < 170) {
		_lineIncrement = 1;

		if (!getScene()->matteBarHeight) {
			getCursor()->hide();
			getScene()->makeGreyPalette();
			getScene()->matteVar1 = 1;
			getScene()->matteBarHeight = 1;
			getScene()->matteVar2 = 0;
			getScene()->mattePlaySound = (cmd->param3 == 0);
			getScene()->matteInitialized = (cmd->param2 == 0);
			_delayedVideoIndex = cmd->param1;
		}

		return;
	}

	bool check = false;
	ActionArea *area = getWorld()->actions[getScene()->getActor()->getActionIndex3()];
	if (area->paletteResourceId) {
		getScreen()->setPalette(area->paletteResourceId);
		getScreen()->setGammaLevel(area->paletteResourceId, 0);
	} else {
		getScreen()->setPalette(getWorld()->currentPaletteId);
		getScreen()->setGammaLevel(getWorld()->currentPaletteId, 0);
	}

	getScene()->matteBarHeight = 0;
	_lineIncrement = 0;

	if (!getScene()->mattePlaySound && _currentScript->commands[0].numLines != 0) {
		bool found = true;
		int index = 0;


		while (_currentScript->commands[index].opcode != 0x2B) { // ChangeScene
			++index;

			if (index >= _currentScript->commands[0].numLines) {
				found = false;
				break;
			}
		}

		if (found)
			check = true;
	}

	if (!check && getScene()->matteVar2 == 0 && getWorld()->musicCurrentResourceIndex != kMusicStopped)
		_vm->sound()->playMusic(MAKE_RESOURCE(kResourcePackMusic, getWorld()->musicCurrentResourceIndex));

	getCursor()->show();
	getScene()->matteVar2 = 0;
}

//////////////////////////////////////////////////////////////////////////
// Opcode 0x2E
IMPLEMENT_OPCODE(StopAllObjectsSounds) {
	Object *object = getWorld()->getObjectById((ObjectId)cmd->param1);

	object->stopAllSounds();
}

//////////////////////////////////////////////////////////////////////////
// Opcode 0x2F
IMPLEMENT_OPCODE(StopProcessing) {
	_skipProcessing = true;
}

//////////////////////////////////////////////////////////////////////////
// Opcode 0x30
IMPLEMENT_OPCODE(ResumeProcessing) {
	_skipProcessing = false;
}

//////////////////////////////////////////////////////////////////////////
// Opcode 0x31
IMPLEMENT_OPCODE(ResetSceneRect) {
	getWorld()->sceneRectIdx = LOBYTE(cmd->param1);
	getScreen()->paletteFade(0, 25, 10);
	_vm->setFlag(kFlagTypeSceneRectChanged);

	getWorld()->xLeft = getWorld()->sceneRects[getWorld()->sceneRectIdx].left;
	getWorld()->yTop  = getWorld()->sceneRects[getWorld()->sceneRectIdx].top;
}

//////////////////////////////////////////////////////////////////////////
// Opcode 0x32
IMPLEMENT_OPCODE(ChangeMusicById) {
	_vm->sound()->changeMusic((ResourceId)cmd->param1, cmd->param2 ? 2 : 1);
}

//////////////////////////////////////////////////////////////////////////
// Opcode 0x33
IMPLEMENT_OPCODE(StopMusic) {
	_vm->sound()->changeMusic(kMusicStopped, 0);
}

//////////////////////////////////////////////////////////////////////////
// Opcode 0x34
IMPLEMENT_OPCODE(_unk34_Status) {
	if (cmd->param1 >= 2) {
		cmd->param1 = 0;
	} else {
		cmd->param1++;
		_lineIncrement = 1;
	}
}

//////////////////////////////////////////////////////////////////////////
// Opcode 0x35
IMPLEMENT_OPCODE(SetVolume) {
	AmbientSoundItem item = getWorld()->ambientSounds[cmd->param1];
	int var = cmd->param2 + item.field_C;

	double volume = -((Config.sfxVolume + var) * (Config.ambientVolume + var));

	if (volume < 0) {
		if (volume < -10000)
			volume = -10000;
	} else {
		volume = 0;
	}

	_vm->sound()->setVolume(item.resourceId, volume);
}

//////////////////////////////////////////////////////////////////////////
// Opcode 0x36
IMPLEMENT_OPCODE(Jump) {
	_currentLine = cmd->param1 - 1;
}

//////////////////////////////////////////////////////////////////////////
// Opcode 0x37
IMPLEMENT_OPCODE(RunBlowUpPuzzle) {
	getScreen()->clearScreen();
	getScreen()->clearGraphicsInQueue();

	_vm->switchMessageHandler(_vm->getMessageHandler(cmd->param1));

	_currentLine++;

	_exit = true;
}

//////////////////////////////////////////////////////////////////////////
// Opcode 0x38
IMPLEMENT_OPCODE(JumpIfAction8) {
	jumpIfActionFlag(cmd, kActionType8);
}

//////////////////////////////////////////////////////////////////////////
// Opcode 0x39
IMPLEMENT_OPCODE(SetAction8) {
	setActionFlag(cmd, kActionType8);
}

//////////////////////////////////////////////////////////////////////////
// Opcode 0x3A
IMPLEMENT_OPCODE(ClearAction8) {
	clearActionFlag(cmd, kActionType8);
}

//////////////////////////////////////////////////////////////////////////
// Opcode 0x3B
IMPLEMENT_OPCODE(_unk3B_PALETTE_MOD) {
	if (!cmd->param2) {
		getScene()->makeGreyPalette();
		cmd->param2 = 1;
	}

	if (cmd->param1 >= 22) {
		getScreen()->clearScreen();

		cmd->param1 = 0;
		cmd->param2 = 0;
		_currentLine++;

		getScreen()->clearGraphicsInQueue();

		_exit = true;
		return;
	}

	getScene()->updatePalette(cmd->param1);

	_lineIncrement = 1;
	++cmd->param1;
}

//////////////////////////////////////////////////////////////////////////
// Opcode 0x3C
IMPLEMENT_OPCODE(IncrementParam2) {
	if (cmd->param1) {
		if (cmd->param2 >= cmd->param1) {
			cmd->param2 = 0;
		} else {
			++cmd->param2;
			_lineIncrement = 1;
		}
	}
}

//////////////////////////////////////////////////////////////////////////
// Opcode 0x3D
IMPLEMENT_OPCODE(WaitUntilFramePlayed) {
	Object *object = getWorld()->getObjectById((ObjectId)cmd->param1);

	int32 frameNum = cmd->param2;
	if (frameNum == -1)
		frameNum = object->getFrameCount() - 1;

	if ((int32)object->getFrameIndex() != frameNum) {
		_lineIncrement = 1;
		_waitCycle     = true;
	}
}


//////////////////////////////////////////////////////////////////////////
// Opcode 0x3E
IMPLEMENT_OPCODE(UpdateWideScreen) {
	int barSize = cmd->param1;

	if (barSize >= 22) {
		cmd->param1 = 0;
		_lineIncrement = 0;

		getScene()->matteBarHeight = 0;
	} else {
		getScreen()->drawWideScreen(4 * barSize);

		_lineIncrement = 1;
		++cmd->param1;
	}
}


//////////////////////////////////////////////////////////////////////////
// Opcode 0x3F
IMPLEMENT_OPCODE(JumpIfActor) {
	ActorIndex index = (cmd->param1 == kActorInvalid) ? getScene()->getPlayerActorIndex() : cmd->param1;

	if (_currentQueueEntry.actorIndex != index)
		_currentLine = cmd->param2 - 1;
}

//////////////////////////////////////////////////////////////////////////
// Opcode 0x40
IMPLEMENT_OPCODE(PlaySpeechScene) {
	if (cmd->param1 < 0 || cmd->param1 >= 5)
		return;

	if (cmd->param4 != 2) {
		cmd->param5 = getSpeech()->playIndexed(cmd->param1);

		if (cmd->param2) {
			_vm->setGameFlag(kGameFlagScriptProcessing);
			cmd->param4 = 2;
			if (cmd->param6) {
				_vm->setFlag(kFlagType1);
				_vm->setFlag(kFlagType2);
			}
			_lineIncrement = 1;
		}

		if (cmd->param3 && !cmd->param6)
			_vm->setGameFlag(kGameFlag219);

		return;
	}

	if (_vm->sound()->isPlaying((ResourceId)cmd->param5)) {
		_lineIncrement = 1;
		return;
	}

	_vm->clearGameFlag(kGameFlagScriptProcessing);
	cmd->param4 = 0;

	if (cmd->param3) {
		if (cmd->param6) {
			_vm->clearFlag(kFlagType1);
			_vm->clearFlag(kFlagType2);

			return;
		} else {
			_vm->clearGameFlag(kGameFlag219);
		}
	}

	if (!cmd->param6) {
		cmd->param6 = 1;
	} else {
		_vm->clearFlag(kFlagType1);
		_vm->clearFlag(kFlagType2);
	}
}

//////////////////////////////////////////////////////////////////////////
// Opcode 0x41
IMPLEMENT_OPCODE(PlaySpeech) {
	if (cmd->param1 < 0)
		return;

	if (cmd->param4 != 2) {
		cmd->param5 = getSpeech()->playPlayer((ResourceId)cmd->param1);

		if (cmd->param2) {
			_vm->setGameFlag(kGameFlagScriptProcessing);
			cmd->param4 = 2;
			if (cmd->param6) {
				_vm->setFlag(kFlagType1);
				_vm->setFlag(kFlagType2);
			}
			_lineIncrement = 1;
		}

		if (cmd->param3 && !cmd->param6)
			_vm->setGameFlag(kGameFlag219);

		return;
	}

	if (_vm->sound()->isPlaying((ResourceId)cmd->param5)) {
		_lineIncrement = 1;
		return;
	}

	_vm->clearGameFlag(kGameFlagScriptProcessing);
	cmd->param4 = 0;

	if (cmd->param3) {
		if (cmd->param6) {
			_vm->clearFlag(kFlagType1);
			_vm->clearFlag(kFlagType2);

			return;
		} else {
			_vm->clearGameFlag(kGameFlag219);
		}
	}

	if (!cmd->param6) {
		cmd->param6 = 1;
	} else {
		_vm->clearFlag(kFlagType1);
		_vm->clearFlag(kFlagType2);
	}
}

//////////////////////////////////////////////////////////////////////////
// Opcode 0x42
IMPLEMENT_OPCODE(PlaySpeechScene2) {
	if (cmd->param1 < 0 || cmd->param1 >= 20 || cmd->param2 < 0)
		return;

	if (cmd->param5 == 2) {
		if (getSound()->isPlaying((ResourceId)cmd->param6)) {
			_lineIncrement = 1;
			return;
		}

		_vm->clearGameFlag(kGameFlagScriptProcessing);

		cmd->param5 = 0;

		if (!cmd->param4) {
			if (!cmd->param7) {
				cmd->param7 = 1;
				return;
			}
		} if (!cmd->param7) {
			_vm->clearGameFlag(kGameFlag219);
			cmd->param7 = 1;
			return;
		}

		_vm->clearFlag(kFlagType1);
		_vm->clearFlag(kFlagType2);
		return;
	}

	cmd->param6 = getSpeech()->playScene(cmd->param1, cmd->param2);

	if (cmd->param3) {
		_vm->setGameFlag(kGameFlagScriptProcessing);

		cmd->param5 = 2;

		if (cmd->param7) {
			_vm->clearFlag(kFlagType1);
			_vm->clearFlag(kFlagType2);
		}

		_lineIncrement = 1;
	}

	if (cmd->param4 && !cmd->param7)
		_vm->setGameFlag(kGameFlag219);
}

//////////////////////////////////////////////////////////////////////////
// Opcode 0x43
IMPLEMENT_OPCODE(MoveScenePositionFromActor) {
	Actor *actor = getScene()->getActor(cmd->param1);

	if (!cmd->param3) {
		getWorld()->motionStatus = 5;

		getScene()->updateSceneCoordinates(actor->x1 + Common::Rational(actor->x2, 2).toInt() - 320,
			                           actor->y1 + Common::Rational(actor->y2, 2).toInt() - 240,
		                               cmd->param2);
	} else if (cmd->param6) {
		if (getWorld()->motionStatus == 2) {
			_lineIncrement = 1;
		} else {
			cmd->param6 = 0;
			getWorld()->coordinates[0] = -1;
		}
	} else {
		cmd->param6 = 1;
		getWorld()->motionStatus = 2;

		if (getScene()->updateSceneCoordinates(actor->x1 + Common::Rational(actor->x2, 2).toInt() - 320,
										   actor->y1 + Common::Rational(actor->y2, 2).toInt() - 240,
										   cmd->param2,
										   true,
										   &cmd->param6))
			_lineIncrement = 0;
		else
			_lineIncrement = 1;
	}
}

//////////////////////////////////////////////////////////////////////////
// Opcode 0x44
IMPLEMENT_OPCODE(PaletteFade) {
	getScreen()->paletteFade(0, cmd->param1, cmd->param2);
}

//////////////////////////////////////////////////////////////////////////
// Opcode 0x45
IMPLEMENT_OPCODE(StartPaletteFadeThread) {
	getScreen()->startPaletteFade(getWorld()->currentPaletteId, cmd->param1, cmd->param2);
}

//////////////////////////////////////////////////////////////////////////
// Opcode 0x46
IMPLEMENT_OPCODE(_unk46) {
	if (cmd->param6) {
		if (getSound()->isPlaying(getSpeech()->getSoundResourceId())) {
			_lineIncrement = 1;
		} else {
			cmd->param6 = 0;
			if (cmd->param5) {
				getScene()->getActor(cmd->param5)->updateStatus(kActorStatusEnabled);
			} else if (cmd->param4 != cmd->param3 && cmd->param4) {
				getWorld()->getObjectById((ObjectId)cmd->param3)->disable();

				Object *object = getWorld()->getObjectById((ObjectId)cmd->param4);
				object->setNextFrame(object->flags);
			}

			_vm->clearGameFlag(kGameFlagScriptProcessing);

			getSpeech()->resetResourceIds();
		}
	} else {
		_vm->setGameFlag(kGameFlagScriptProcessing);
		getSpeech()->play(MAKE_RESOURCE(kResourcePackSpeech, 515 + cmd->param1), MAKE_RESOURCE(kResourcePackShared, 1290 + cmd->param1));

		if (cmd->param2) {
			getScene()->getActor(cmd->param5)->updateStatus(kActorStatus8);
			cmd->param6 = 1;
			_lineIncrement = 1;
		} else {
			if (cmd->param4 != cmd->param3) {
				if (cmd->param4)
					getWorld()->getObjectById((ObjectId)cmd->param4)->disable();

				if (cmd->param3)
					getWorld()->getObjectById((ObjectId)cmd->param3)->setNextFrame(getWorld()->getObjectById((ObjectId)cmd->param4)->flags);
			}

			cmd->param6 = 1;
			_lineIncrement = 1;
		}
	}
}

//////////////////////////////////////////////////////////////////////////
// Opcode 0x47
IMPLEMENT_OPCODE(ActorFaceObject) {
	getScene()->getActor(cmd->param1)->faceTarget((ObjectId)cmd->param2, (DirectionFrom)cmd->param3);
}

//////////////////////////////////////////////////////////////////////////
// Opcode 0x48
IMPLEMENT_OPCODE(_unk48_MATTE_01) {
	getScene()->matteVar1 = 0;
	getScene()->matteInitialized = true;

	if (getScene()->matteBarHeight >= 170) {
		getScene()->matteBarHeight = 0;
		_lineIncrement = 0;
		getCursor()->show();
	} else {
		_lineIncrement = 1;

		if (!getScene()->matteBarHeight) {
			getCursor()->hide();
			getScene()->matteBarHeight = 1;
		}
	}
}

//////////////////////////////////////////////////////////////////////////
// Opcode 0x49
IMPLEMENT_OPCODE(_unk49_MATTE_90) {
	getScene()->matteVar1 = 0;
	getScene()->matteInitialized = true;
	getScene()->mattePlaySound = true;

	if (getScene()->matteBarHeight >= 170) {
		getScene()->matteBarHeight = 0;
		_lineIncrement = 0;
		getCursor()->show();
	} else {
		_lineIncrement = 1;

		if (!getScene()->matteBarHeight) {
			getCursor()->hide();
			getScene()->matteBarHeight = 90;
		}
	}
}

//////////////////////////////////////////////////////////////////////////
// Opcode 0x4A
IMPLEMENT_OPCODE(JumpIfSoundPlaying) {
	if (cmd->param3 == 1) {
		if (_vm->sound()->isPlaying((ResourceId)cmd->param1)) {
			_currentLine = cmd->param2;
		}
	} else if (!_vm->sound()->isPlaying((ResourceId)cmd->param1)) {
		_currentLine = cmd->param2;
	}
}

//////////////////////////////////////////////////////////////////////////
// Opcode 0x4B
IMPLEMENT_OPCODE(ChangePlayerActorIndex) {
	getScene()->changePlayer(cmd->param1);
}

//////////////////////////////////////////////////////////////////////////
// Opcode 0x4C
IMPLEMENT_OPCODE(ChangeActorStatus) {
	Actor *actor = getScene()->getActor(cmd->param1);

	if (cmd->param2) {
		if (actor->getStatus() < kActorStatus11)
			actor->setStatus(kActorStatus14);
	} else {
		actor->setStatus(kActorStatusEnabled);
	}
}

//////////////////////////////////////////////////////////////////////////
// Opcode 0x4D
IMPLEMENT_OPCODE(StopSound) {
	if (_vm->sound()->isPlaying((ResourceId)cmd->param1))
		_vm->sound()->stopSound((ResourceId)cmd->param1);
}

//////////////////////////////////////////////////////////////////////////
// Opcode 0x4E
IMPLEMENT_OPCODE(JumpRandom) {
	if (_vm->getRandom(cmd->param1) < (uint32)cmd->param2)
		return;

	setNextLine(cmd->param3);
}

//////////////////////////////////////////////////////////////////////////
// Opcode 0x4F
IMPLEMENT_OPCODE(ClearScreen) {
	getScene()->setSkipDrawScene(cmd->param1);

	if (cmd->param1)
		getScreen()->clearScreen();
}

//////////////////////////////////////////////////////////////////////////
// Opcode 0x50
IMPLEMENT_OPCODE(Quit) {
	getScreen()->clearScreen();
	_vm->quitGame();

	// We need to exit the interpreter loop so we get back to the event loop
	// and get the quit message
	_exit = true;
}

//////////////////////////////////////////////////////////////////////////
// Opcode 0x51
IMPLEMENT_OPCODE(JumpObjectFrame) {
	Object *object = getWorld()->getObjectById((ObjectId)cmd->param1);

	if (cmd->param2 == -1)
		cmd->param2 = object->getFrameCount() - 1;

	if (cmd->param3) {
		if(object->getFrameIndex() == (uint32)cmd->param2)
			return;
	} else if (cmd->param4) {
		if (object->getFrameIndex() < (uint32)cmd->param2)
			return;
	} else if (cmd->param5) {
		if (object->getFrameIndex() > (uint32)cmd->param2)
			return;
	} else if (cmd->param6) {
		if (object->getFrameIndex() <= (uint32)cmd->param2)
			return;
	} else if (cmd->param7) {
		if (object->getFrameIndex() >= (uint32)cmd->param2)
			return;
	} else if (!cmd->param8 || object->getFrameIndex() != (uint32)cmd->param2) {
		return;
	}

	setNextLine(cmd->param9);
}

//////////////////////////////////////////////////////////////////////////
// Opcode 0x52
IMPLEMENT_OPCODE(DeleteGraphics) {
	for (uint i = 0; i < 55; i++)
		getScreen()->deleteGraphicFromQueue(getScene()->getActor(cmd->param1)->getResourcesId(cmd->param1));
}

//////////////////////////////////////////////////////////////////////////
// Opcode 0x53
IMPLEMENT_OPCODE(SetActorField944) {
	Actor *actor = getScene()->getActor(cmd->param1);

	actor->setField944(cmd->param2);
}

//////////////////////////////////////////////////////////////////////////
// Opcode 0x54
IMPLEMENT_OPCODE(_unk54_SET_ACTIONLIST_6EC) {
	if (cmd->param2)
		_currentScript->field_1BB0 = _vm->getRandom(cmd->param1);
	else
		_currentScript->field_1BB0 = cmd->param1;
}

//////////////////////////////////////////////////////////////////////////
// Opcode 0x55
IMPLEMENT_OPCODE(_unk55) {

	if (cmd->param2) {
		if (_currentScript->field_1BB0 == cmd->param1)
			return;
	} else {

		if (cmd->param3) {
			if (_currentScript->field_1BB0 < cmd->param1)
				return;
		} else if (cmd->param4) {
			if (_currentScript->field_1BB0 > cmd->param1)
				return;
		} else if (!cmd->param5) {
			if (cmd->param6) {
				if (_currentScript->field_1BB0 >= cmd->param1)
					return;
			} else {
				if (!cmd->param7 || _currentScript->field_1BB0 != cmd->param1)
					return;
			}
		} else {
			if (_currentScript->field_1BB0 <= cmd->param1)
				return;
		}
	}

	setNextLine(cmd->param8);
}

//////////////////////////////////////////////////////////////////////////
// Opcode 0x56
IMPLEMENT_OPCODE(_unk56) {
	Actor *actor = getScene()->getActor(cmd->param2 == 2 ? kActorInvalid : cmd->param1);

	if (actor->getStatus() == kActorStatus2 || actor->getStatus() == kActorStatus13) {
		if (cmd->param2 == 2)
			_lineIncrement = 1;

		return;
	}

	if (cmd->param2 == 2) {
		cmd->param2 = 1;
		_lineIncrement = 0;

		if ((actor->x1 + actor->x2 == cmd->param6) && (actor->y1 + actor->y2 == cmd->param7)) {
			getScene()->getActor()->faceTarget((ObjectId)cmd->param1, kDirectionFromActor);
			actor->updateFromDirection((actor->getDirection() + 4) & 7);
		} else {
			_currentLine = cmd->param3;
		}
	} else {
		int32 x = 0;
		int32 y = 0; // FIXME: is is set somewhere else?

		if (getScene()->processActor(&x, &cmd->param4) == 1) {
			getScene()->getActor()->processStatus(x, y, cmd->param4);
			cmd->param6 = x;
			cmd->param7 = y;

			if (cmd->param2 == 1) {
				cmd->param2 = 2;
				_lineIncrement = 1;
			}
		} else {
			if (cmd->param4)
				getSpeech()->playIndexed(1);

			_currentLine = cmd->param3;
		}
	}
}

//////////////////////////////////////////////////////////////////////////
// Opcode 0x57
IMPLEMENT_OPCODE(SetResourcePalette) {
	getWorld()->currentPaletteId = getWorld()->graphicResourceIds[cmd->param1];
	getScreen()->setPalette(getWorld()->currentPaletteId);
	getScreen()->setGammaLevel(getWorld()->currentPaletteId, 0);
}

//////////////////////////////////////////////////////////////////////////
// Opcode 0x58
IMPLEMENT_OPCODE(SetObjectFrameIdxFlaged) {
	Object *object = getWorld()->getObjectById((ObjectId)cmd->param1);

	if (cmd->param3)
		object->flags = 1 | object->flags;
	else
		object->flags &= ~kObjectFlagEnabled;

	object->setFrameIndex(cmd->param2);
}

//////////////////////////////////////////////////////////////////////////
// Opcode 0x59
IMPLEMENT_OPCODE(_unk59) {
	Object *object = getWorld()->getObjectById((ObjectId)cmd->param1);

	if (cmd->param2) {
		object->flags |= kObjectFlag40000;
	} else {
		object->flags &= ~kObjectFlag10E38;
	}

	if (cmd->param3 && (object->flags & kObjectFlag10E38))
		_lineIncrement = 1;
}

//////////////////////////////////////////////////////////////////////////
// Opcode 0x5A
IMPLEMENT_OPCODE(_unk5A) {
	getScene()->getActor(cmd->param1)->setActionIndex2(cmd->param2);
}

//////////////////////////////////////////////////////////////////////////
// Opcode 0x5B
IMPLEMENT_OPCODE(_unk5B) {
	if (cmd->param2 >= 0 && cmd->param2 <= 3) {
		if (cmd->param1) {
			Object *object = getWorld()->getObjectById((ObjectId)cmd->param1);

			object->setField67C(cmd->param2);

			if (object->getField67C())
				object->setField67C(object->getField67C() + 3);
		} else {
			getScene()->getActor(cmd->param3)->setField96C(cmd->param2);
		}
	}
}

//////////////////////////////////////////////////////////////////////////
// Opcode 0x5C
IMPLEMENT_OPCODE(QueueScript) {
	queueScript(getWorld()->getActionAreaById(cmd->param1)->scriptIndex, cmd->param2);
}

//////////////////////////////////////////////////////////////////////////
// Opcode 0x5D
IMPLEMENT_OPCODE(_unk5D) {
	Actor *actor = getScene()->getActor(cmd->param1);

	actor->process_401830(cmd->param2, cmd->param3, cmd->param4, cmd->param5, cmd->param6, cmd->param7, cmd->param8, cmd->param9);
}

//////////////////////////////////////////////////////////////////////////
// Opcode 0x5E
IMPLEMENT_OPCODE(ClearActorFields) {
	Actor *actor = getScene()->getActor(cmd->param1);

	// Clear fields starting from field_970
	actor->clearFields();
}

//////////////////////////////////////////////////////////////////////////
// Opcode 0x5F
IMPLEMENT_OPCODE(SetObjectLastFrameIdx) {
	Object *object = getWorld()->getObjectById((ObjectId)cmd->param1);

	if (object->getFrameIndex() == object->getFrameCount() - 1) {
		_lineIncrement = 0;
		object->flags &= ~kObjectFlag10E38;
	} else {
		_lineIncrement = 1;
	}
}

//////////////////////////////////////////////////////////////////////////
// Opcode 0x60
IMPLEMENT_OPCODE(_unk60_SET_OR_CLR_ACTIONAREA_FLAG) {
	ActionArea *area = getWorld()->getActionAreaById(cmd->param1);

	if (cmd->param2)
		area->flags |= 1;
	else
		area->flags &= ~1;
}

//////////////////////////////////////////////////////////////////////////
// Opcode 0x61
IMPLEMENT_OPCODE(_unk61) {
	if (cmd->param2) {
		if (getWorld()->field_E860C == -1) {
			_lineIncrement = 0;
			cmd->param2 = 0;
		} else {
			_lineIncrement = 1;
		}
	} else {
		getScene()->updatePlayerChapter9(cmd->param1);
		cmd->param2 = 1;
		_lineIncrement = 1;
	}
}

//////////////////////////////////////////////////////////////////////////
// Opcode 0x62
IMPLEMENT_OPCODE(ShowOptionsScreen) {
	_vm->menu()->showOptions();
}

//////////////////////////////////////////////////////////////////////////
// Opcode 0x63
IMPLEMENT_OPCODE(_unk63) {
	if (cmd->param1) {
		_vm->setFlag(kFlagType1);
		_vm->setFlag(kFlagType2);
	}

	if (_vm->sound()->isPlaying(getSpeech()->getSoundResourceId())) {
		_lineIncrement = 1;
		return;
	} else if (!cmd->param1) {
		cmd->param1 = 1;
	}
}

//////////////////////////////////////////////////////////////////////////
// Opcode Helper functions
//////////////////////////////////////////////////////////////////////////

void ActionList::enableObject(ScriptEntry *cmd, ObjectEnableType type) {
	error("[ActionList::enableObject] not implemented!");
}

void ActionList::setActionFlag(ScriptEntry *cmd, ActionType flag) {
	switch (cmd->param2) {
	default:
		getWorld()->getObjectById((ObjectId)cmd->param1)->actionType |= flag;
		break;

	case 1:
		getWorld()->getActionAreaById(cmd->param1)->actionType |= flag;
		break;

	case 2:
		getWorld()->actors[cmd->param1]->actionType |= flag;
		break;
	}
}

void ActionList::clearActionFlag(ScriptEntry *cmd, ActionType flag) {
	switch (cmd->param2) {
	default:
		getWorld()->getObjectById((ObjectId)cmd->param1)->actionType &= ~flag;
		break;

	case 1:
		getWorld()->getActionAreaById(cmd->param1)->actionType &= ~flag;
		break;

	case 2:
		getWorld()->actors[cmd->param1]->actionType &= ~flag;
		break;
	}
}

void ActionList::jumpIfActionFlag(ScriptEntry *cmd, ActionType flag) {
	bool done = false;

	switch (cmd->param3) {
	default:
		done = (getWorld()->actors[cmd->param1]->actionType & flag) == 0;
		break;

	case 0:
		done = (getWorld()->getObjectById((ObjectId)cmd->param1)->actionType & flag) == 0;
		break;

	case 1:
		done = (getWorld()->getActionAreaById(cmd->param1)->actionType & flag) == 0;
		break;
	}

	if (!done)
		return;

	setNextLine(cmd->param2);
}

void ActionList::setNextLine(int32 line) {
	int32 opcode = _currentScript->commands[line].opcode;
	if (opcode == 0x10 || opcode == 0) { // Return
		_currentLine = line;
	} else {
		_done = true;
	}
}

} // end of namespace Asylum

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

#include "common/config-manager.h"
#include "common/events.h"
#include "common/system.h"
#include "common/file.h"

#include "asylum/asylum.h"

namespace Asylum {

enum EyesAnimation {
	kEyesFront = 0,
	kEyesLeft = 1,
	kEyesRight = 2,
	kEyesTop = 3,
	kEyesBottom = 4,
	kEyesTopLeft = 5,
	kEyesTopRight = 6,
	kEyesBottomLeft = 7,
	kEyesBottomRight = 8,
	kEyesCrossed = 9
};

AsylumEngine::AsylumEngine(OSystem *system, Common::Language language)
    : Engine(system) {

    Common::File::addDefaultDirectory(_gameDataDir.getChild("Data"));
    Common::File::addDefaultDirectory(_gameDataDir.getChild("Vids"));
	Common::File::addDefaultDirectory(_gameDataDir.getChild("Music"));

    _eventMan->registerRandomSource(_rnd, "asylum");
}

AsylumEngine::~AsylumEngine() {
    //Common::clearAllDebugChannels();
	delete _scene;
    delete _resMgr;
	_backBuffer.free();
}

Common::Error AsylumEngine::run() {
    Common::Error err;
    err = init();
    if (err != Common::kNoError)
            return err;
    return go();
}

// Will do the same as subroutine at address 0041A500
Common::Error AsylumEngine::init() {
	// initialize engine objects

	initGraphics(640, 480, true);
	_backBuffer.create(640, 480, 1);

	_resMgr = new ResourceManager(this);
    _scene = new Scene(this);

	// initializing game
	// TODO: save dialogue key codes into sntrm_k.txt (need to figure out why they use such thing)
	// TODO: get hand icon resource before starting main menu
	// TODO: load startup configurations (address 0041A970)
	// TODO: setup cinematics (address 0041A880) (probably we won't need it)
	// TODO: init unknown game stuffs (address 0040F430)

	// TODO: if savegame exists on folder, than start NewGame()

    return Common::kNoError;
}

Common::Error AsylumEngine::go() {
	_mouseX = 0, _mouseY = 0;
	_activeIcon = -1;
	_previousActiveIcon = -1;
	_curIconFrame = 0;
	_curMouseCursor = 0;
	_cursorStep = 1;

	// Play intro movie
	// Disabled for quick testing
	//_resMgr->loadVideo(0);

    // TODO: just some scene proof-of-concept
    _scene->load(5);

	showMainMenu();

	// DEBUG
    // Control loop test. Basically just keep the
    // ScummVM window alive until ESC is pressed.
    // This will facilitate drawing tests ;)

	// DEBUG
	uint32 lastUpdate = 0;

	while (!shouldQuit()) {
		checkForEvent();

		// Copy background image
		_system->copyRectToScreen((byte *)_backBuffer.pixels, _backBuffer.w, 0, 0, _backBuffer.w, _backBuffer.h);

		updateMainMenu();

		updateMouseCursor();

		waitForTimer(60);
	}

    return Common::kNoError;
}

void AsylumEngine::updateMouseCursor() {
	_curMouseCursor += _cursorStep;
	if (_curMouseCursor == 0)
		_cursorStep = 1;
	if (_curMouseCursor == 6)
		_cursorStep = -1;

	_resMgr->loadCursor(1, 2, _curMouseCursor);
}

void AsylumEngine::waitForTimer(int msec_delay) {
	uint32 start_time = _system->getMillis();

	while (_system->getMillis() < start_time + msec_delay) {
		checkForEvent();
		_system->updateScreen();
		_system->delayMillis(10);
	}
}

void AsylumEngine::checkForEvent() {
	Common::Event ev;

	if (_system->getEventManager()->pollEvent(ev)) {
		if (ev.type == Common::EVENT_KEYDOWN) {
			if (ev.kbd.keycode == Common::KEYCODE_ESCAPE) {
				// Push a quit event
				Common::Event event;
				event.type = Common::EVENT_QUIT;
				g_system->getEventManager()->pushEvent(event);
			}
			//if (ev.kbd.keycode == Common::KEYCODE_RETURN)
		} else if (ev.type == Common::EVENT_MOUSEMOVE) {
			_mouseX = ev.mouse.x;
			_mouseY = ev.mouse.y;
		}
	}
}

void AsylumEngine::showMainMenu() {
	// main menu background
	_resMgr->loadGraphic(1, 0, 0);
	_resMgr->loadPalette(1, 17);
	_resMgr->loadCursor(1, 2, 0);
	_resMgr->loadMusic();
}

void AsylumEngine::updateMainMenu() {
	// TODO: Just some proof-of concept to change icons here for now
	if (_mouseY >= 20 && _mouseY <= 20 + 48) {
		// Top row
		for (int i = 0; i <= 5; i++) {
			int curX = 40 + i * 100;
			if (_mouseX >= curX && _mouseX <= curX + 55) {
				GraphicResource *res = _resMgr->getGraphic(1, i + 4, _curIconFrame);
				_system->copyRectToScreen(res->data, res->width, curX, 20, res->width, res->height);

				// Cycle icon frame
				// Icon animations have 15 frames, 0-14
				_curIconFrame++;
				if (_curIconFrame == 15)
					_curIconFrame = 0;

				_activeIcon = i;

				// Play creepy voice
				if (!_mixer->isSoundHandleActive(_sfxHandle) && _activeIcon != _previousActiveIcon) {
					_mixer->playInputStream(Audio::Mixer::kSFXSoundType, &_sfxHandle, _resMgr->loadSFX(1, i + 44));
					_previousActiveIcon = _activeIcon;
				}

				break;
			}
		}
	} else if (_mouseY >= 400 && _mouseY <= 400 + 48) {
		// Bottom row
		for (int i = 0; i <= 5; i++) {
			int curX = 40 + i * 100;
			if (_mouseX >= curX && _mouseX <= curX + 55) {
				int iconNum = i + 10;

				// The last 2 icons are swapped
				if (iconNum == 14)
					iconNum = 15;
				else if (iconNum == 15)
					iconNum = 14;

				GraphicResource *res = _resMgr->getGraphic(1, iconNum, _curIconFrame);
				_system->copyRectToScreen(res->data, res->width, curX, 400, res->width, res->height);

				// Cycle icon frame
				// Icon animations have 15 frames, 0-14
				_curIconFrame++;
				if (_curIconFrame == 15)
					_curIconFrame = 0;

				_activeIcon = i + 6;

				// HACK: the credits icon has less frames (0 - 9). Currently, there's no way to find the number
				// of frames with the current resource manager implementation, so we just hardcode it here
				if (_activeIcon == 10 && _curIconFrame >= 10)
					_curIconFrame = 0;

				// Play creepy voice
				if (!_mixer->isSoundHandleActive(_sfxHandle) && _activeIcon != _previousActiveIcon) {
					_mixer->playInputStream(Audio::Mixer::kSFXSoundType, &_sfxHandle, _resMgr->loadSFX(1, iconNum + 40));
					_previousActiveIcon = _activeIcon;
				}

				break;
			}
		}
	} else {
		// No selection
		_previousActiveIcon = _activeIcon = -1;
	}

	// Eyes animation
	// Get the appropriate eye resource depending on the mouse position
	int eyeResource = kEyesFront;

	if (_mouseX <= 200) {
		if (_mouseY <= 160)
			eyeResource = kEyesTopLeft;
		else if (_mouseY > 160 && _mouseY <= 320)
			eyeResource = kEyesLeft;
		else
			eyeResource = kEyesBottomLeft;
	} else if (_mouseX > 200 && _mouseX <= 400) {
		if (_mouseY <= 160)
			eyeResource = kEyesTop;
		else if (_mouseY > 160 && _mouseY <= 320)
			eyeResource = kEyesFront;
		else
			eyeResource = kEyesBottom;
	} else if (_mouseX > 400) {
		if (_mouseY <= 160)
			eyeResource = kEyesTopRight;
		else if (_mouseY > 160 && _mouseY <= 320)
			eyeResource = kEyesRight;
		else
			eyeResource = kEyesBottomRight;
	}
	// TODO: kEyesCrossed state

	GraphicResource *res = _resMgr->getGraphic(1, 1, eyeResource);
	copyRectToScreenWithTransparency(res->data, 265, 230, res->width, res->height);
}

void AsylumEngine::copyToBackBuffer(byte *buffer, int x, int y, int width, int height) {
	int h = height;
	byte *dest = (byte *)_backBuffer.pixels;

	while (h--) {
		memcpy(dest, buffer, width);
		dest += 640;
		buffer += width;
	}
}

void AsylumEngine::copyRectToScreenWithTransparency(byte *buffer, int x, int y, int width, int height) {
	byte *screenBuffer = (byte *)_system->lockScreen()->pixels;

	for (int curY = 0; curY < height; curY++) {
		for (int curX = 0; curX < width; curX++) {
			if (buffer[curX + curY * width] != 0) {
				screenBuffer[x + curX + (y + curY) * 640] = buffer[curX + curY * width];
			}
		}
	}

	_system->unlockScreen();
}

} // namespace Asylum

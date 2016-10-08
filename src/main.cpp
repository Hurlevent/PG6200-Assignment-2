#include "GameManager.h"
#include <iostream>
#include <memory>

#ifdef _WIN32
#include <Windows.h>
#endif

#define CUSTOM_MODELS

/**
 * Simple program that starts our game manager
 */
int main(int argc, char *argv[]) {
	for (int i=0; i<argc; ++i) {
		std::cout << "Argument " << i << ": " << argv[i] << std::endl;
	}

	const char * bunny = "models/bunny.obj";
	
	std::shared_ptr<GameManager> game;
	game.reset(new GameManager(
#ifdef CUSTOM_MODELS
		(argc > 1) ? argv[1] : bunny
#else
		bunny
#endif
		));
	game->init();
	game->play();
	game.reset();
	return 0;
}

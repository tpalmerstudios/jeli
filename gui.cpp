#include <iostream>
#include <vector>
#include <algorithm>
//#include <cmath>
#include "SDL.h"

#include "map.h"
#include "utils.h"
#include "player.h"
#include "sprite.h"
#include "framebuffer.h"
#include "textures.h"
#include "tinyraycaster.h"

int main ()
{
		FrameBuffer fb {1024, 512, std::vector<uint32_t> (1024 * 512, packColor (255, 255, 255))};
		GameState gs
		{
				Map (),
				{3.456, 2.345, 1.523, M_PI/3., 0, 0, fb.h / 2},
				{ {3.523, 3.812, 2, 0},
				  {1.834, 8.765, 0, 0},
				  {5.323, 5.365, 1, 0},
				  {4.123, 10.76, 1, 0} },
				Texture ("media/wallText-2.png"),
				Texture ("media/monsters.png")
		};
		if (!gs.texWalls.count || !gs.texMonsters.count)
		{
				std::cerr << "Failed to load textures\n";
				return -1;
		}
		SDL_Window *window = nullptr;
		SDL_Renderer *renderer = nullptr;

		if (SDL_Init (SDL_INIT_VIDEO))
		{
				std::cerr << "Couldn't initialize SDL: " << SDL_GetError () << std::endl;
				return -1;
		}
		if (SDL_CreateWindowAndRenderer (fb.w, fb.h, SDL_WINDOW_SHOWN | SDL_WINDOW_INPUT_FOCUS, &window, &renderer))
		{
				std::cerr << "Couldn't create window and renderer: " << SDL_GetError () << std::endl;
				return -1;
		}

		SDL_Texture *frameBufferTexture = SDL_CreateTexture (renderer, SDL_PIXELFORMAT_ABGR8888, SDL_TEXTUREACCESS_STREAMING, fb.w, fb.h);
		SDL_Event event;
		while (1)
		{
				if (SDL_PollEvent (&event))
				{
						// Quit the program if told to or if escape is pressed
						if (event.type == SDL_QUIT || (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_ESCAPE))
								break;
						if (event.key.keysym.sym == 'p')
						{
								dropPPMImage ("frame.ppm", fb.img, fb.w, fb.h);
						}
						if (event.type == SDL_KEYUP)
						{
								// Stop turning on key lift
								if (event.key.keysym.sym == 'a' || event.key.keysym.sym == 'd' || event.key.keysym.sym == SDLK_LEFT || event.key.keysym.sym == SDLK_RIGHT)
										// add <- and -> options
										gs.player.turn = 0;
								// Stop walking on Key lift
								if (event.key.keysym.sym == 'w' || event.key.keysym.sym == 's' || event.key.keysym.sym == SDLK_UP || event.key.keysym.sym == SDLK_DOWN)
										// add /\ and \/ options
										gs.player.walk = 0;
						} // Key up
						if (event.type == SDL_KEYDOWN)
						{
								if (event.key.keysym.sym == 'a' || event.key.keysym.sym == SDLK_LEFT)
										gs.player.turn = -1;
								if (event.key.keysym.sym == 'd' || event.key.keysym.sym == SDLK_RIGHT)
										gs.player.turn = 1;
								if (event.key.keysym.sym == 'w' || event.key.keysym.sym == SDLK_UP) 

										gs.player.walk = 1;
								if (event.key.keysym.sym == 's' || event.key.keysym.sym == SDLK_DOWN)
										gs.player.walk = -1;
						} // Keydown
				} // Event occured
				gs.player.angle += float (gs.player.turn) *.05;
				float newX = gs.player.x + gs.player.walk * cos (gs.player.angle) * 0.1;
				float newY = gs.player.y + gs.player.walk * sin (gs.player.angle) * 0.1;
				if (int (newX) >= 0 && int (newX) < int (gs.map.w) && int (newY) >= 0 && int (newY) < int (gs.map.h) && gs.map.isEmpty (newX, newY))
				{
						gs.player.x = newX;
						gs.player.y = newY;
				} // walking
				for (size_t i =0; i < gs.monsters.size (); ++i)
				{
						gs.monsters [i].playerDist = std::sqrt (pow (gs.player.x - gs.monsters [i].x, 2) + pow (gs.player.y - gs.monsters [i].y, 2));
				} // set monster distance to player
				std::sort (gs.monsters.begin (), gs.monsters.end ()); // sort from farthest to closest (using operator< overload)

				render (fb, gs);
				SDL_UpdateTexture (frameBufferTexture, NULL, reinterpret_cast<void *> (fb.img.data ()), fb.w * 4);
				SDL_RenderClear (renderer);
				SDL_RenderCopy (renderer, frameBufferTexture, NULL, NULL);
				SDL_RenderPresent (renderer);
		} // While (1)
		SDL_DestroyTexture (frameBufferTexture);
		SDL_DestroyRenderer (renderer);
		SDL_DestroyWindow (window);
		SDL_Quit ();
		return 0;
} // main ()

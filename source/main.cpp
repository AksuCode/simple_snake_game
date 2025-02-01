#include <SDL2/SDL.h>

#include <deque>
#include <iostream>
#include <vector>

int main(int argc, char *argv[]) {
  // Initialize SDL:
  SDL_Window *window = nullptr;
  SDL_Renderer *renderer = nullptr;
  SDL_Event event;

  if (SDL_Init(SDL_INIT_VIDEO) < 0) {
    SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Video initialization error: %s\n", SDL_GetError());
    return 1;
  }

  const int width = 1000;
  const int heigth = 1000;

  window = SDL_CreateWindow("Snake",                  // window title
                            SDL_WINDOWPOS_UNDEFINED,  // Window position x
                            SDL_WINDOWPOS_UNDEFINED,  // Window position y
                            width + 10,               // width, in pixels
                            heigth + 10,              // height, in pixels
                            SDL_WINDOW_OPENGL         // flags - see below
  );
  if (window == NULL) {
    SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Could not create window: %s\n", SDL_GetError());
    return 1;
  }

  renderer = SDL_CreateRenderer(window, -1, 0);
  if (renderer == NULL) {
    SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Could not create renderer: %s\n", SDL_GetError());
    return 1;
  }

  // Game logic:
  bool running = true;

  enum Directions { UP, RIGHT, DOWN, LEFT };

  int direction = 0;
  int old_direction = 0;

  // Snake head pos
  SDL_Rect head{width / 2, heigth / 2, 10, 10};

  // Snake body container
  std::deque<SDL_Rect> snake_body;

  // Apples
  std::vector<SDL_Rect> apples;
  int apple_count = 5;
  while (apple_count > 0) {
    int rand_x = (rand() & (width / 10)) * 10;
    int rand_y = (rand() & (heigth / 10)) * 10;
    bool match = false;
    if (head.x == rand_x && head.y == rand_y) {
      match = true;
    }
    for (std::vector<SDL_Rect>::iterator it = apples.begin(); it != apples.end(); it++) {
      if (rand_x == (*it).x && rand_y == (*it).y) {
        match = true;
      }
    }
    if (!match) {
      apples.push_back(SDL_Rect{rand_x, rand_y, 10, 10});
      apple_count -= 1;
    }
  }

  bool win = false;
  while (running) {  // Game loop

    // Check input
    while (SDL_PollEvent(&event)) {
      if (event.type == SDL_QUIT) {
        running = false;
      }
      if (event.type == SDL_KEYDOWN) {
        if (event.key.keysym.sym == SDLK_UP) {
          direction = UP;
        }
        if (event.key.keysym.sym == SDLK_RIGHT) {
          direction = RIGHT;
        }
        if (event.key.keysym.sym == SDLK_DOWN) {
          direction = DOWN;
        }
        if (event.key.keysym.sym == SDLK_LEFT) {
          direction = LEFT;
        }
      }
    }

    if (old_direction == UP && direction == DOWN) {
      direction = old_direction;
    } else if (old_direction == DOWN && direction == UP) {
      direction = old_direction;
    } else if (old_direction == LEFT && direction == RIGHT) {
      direction = old_direction;
    } else if (old_direction == RIGHT && direction == LEFT) {
      direction = old_direction;
    } else {
      old_direction = direction;
    }

    SDL_Rect old_head = head;
    // Move
    switch (direction) {
      case UP:
        head.y -= 10;
        break;
      case RIGHT:
        head.x += 10;
        break;
      case DOWN:
        head.y += 10;
        break;
      case LEFT:
        head.x -= 10;
        break;
    }

    // Modify game state
    if (head.x < 0 || head.x > width || head.y < 0 || head.y > heigth) {
      running = false;
      break;
    }
    for (std::deque<SDL_Rect>::iterator it = snake_body.begin(); it != snake_body.end(); it++) {
      if (head.x == (*it).x && head.y == (*it).y) {
        running = false;
        break;
      }
    }

    bool apple_eaten = false;
    for (std::vector<SDL_Rect>::iterator it = apples.begin(); it != apples.end(); it++) {
      if (head.x == (*it).x && head.y == (*it).y) {
        apples.erase(it);
        apple_eaten = true;
        break;
      }
    }

    if (apple_eaten) {
      snake_body.push_front(old_head);

      int rand_x = (rand() & (width / 10)) * 10;
      int rand_y = (rand() & (heigth / 10)) * 10;
      int tmp_rand_x = rand_x;
      int tmp_rand_y = rand_y;
      int loop = true;
      while (loop) {
        bool match = false;
        if (head.x == tmp_rand_x && head.y == tmp_rand_y) {
          match = true;
        }
        for (std::deque<SDL_Rect>::iterator it = snake_body.begin(); it != snake_body.end(); it++) {
          if (tmp_rand_x == (*it).x && tmp_rand_y == (*it).y) {
            match = true;
          }
        }
        for (std::vector<SDL_Rect>::iterator it = apples.begin(); it != apples.end(); it++) {
          if (tmp_rand_x == (*it).x && tmp_rand_y == (*it).y) {
            match = true;
          }
        }
        if (match) {
          if (tmp_rand_x <= width - 10) {
            tmp_rand_x += 10;
          } else if (tmp_rand_y <= heigth - 10) {
            tmp_rand_x = 0;
            tmp_rand_y += 10;
          } else {
            tmp_rand_x = 0;
            tmp_rand_y = 0;
          }
          if (rand_x == tmp_rand_x && rand_y == tmp_rand_y) {
            win = true;
            running = false;
          }
        } else {
          apples.push_back(SDL_Rect{tmp_rand_x, tmp_rand_y, 10, 10});
          loop = false;
        }
      }
    } else {
      snake_body.push_front(old_head);
      snake_body.pop_back();
    }

    // Clear window
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);

    // Draw body
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_RenderFillRect(renderer, &head);
    for (std::deque<SDL_Rect>::iterator it = snake_body.begin(); it != snake_body.end(); it++) {
      SDL_RenderFillRect(renderer, &(*it));
    }

    // Draw apples
    SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
    for (std::vector<SDL_Rect>::iterator it = apples.begin(); it != apples.end(); it++) {
      SDL_RenderFillRect(renderer, &(*it));
    }

    SDL_RenderPresent(renderer);
    SDL_Delay(200);
  }

  // Finish game
  if (win) {
    std::cout << "VICTORY ROYALE!!!" << std::endl;
  } else {
    std::cout << "TOO BAD!" << std::endl;
  }

  SDL_DestroyWindow(window);
  SDL_Quit();

  return 0;
}
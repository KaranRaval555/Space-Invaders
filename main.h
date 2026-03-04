#include "./base.h"
#include <math.h>


#define SCREEN_WIDTH 1920
#define SCREEN_HEIGHT 1080
#define ALIENS 16

typedef struct {
  Vector2 pos;
  bool isFired;
  Rectangle Area;
} Bullet;

typedef struct {
  Texture2D alienFrames[2];
  Rectangle alienArea;
  Vector2 pos;
  bool isAlive;
} Alien;


void drawAliens(Alien aliens[3][ALIENS], bool up);
void drawAlienRow(Alien aliens[3][ALIENS],u8 row, bool up);
void resetBullet(Bullet *bullets, Rectangle playerArea,u8 bulletWidth , u8 index);
void resetBullets(Bullet *bullets,Rectangle playerArea,u8 bulletWidth , u8 n, u8 width, u8 height);
void moveAliens(Alien aliens[3][ALIENS], i8 alien_velocity);

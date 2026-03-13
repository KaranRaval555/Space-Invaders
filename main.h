#include "./base.h"
#include <math.h>

#define SCREEN_WIDTH 1920
#define SCREEN_HEIGHT 1080
#define ALIENS 16
#define BUNKERS 4
#define BUNKER_ROWS 1
#define BUNKER_COLS 3
#define TOTAL_BULLETS 20

typedef struct {
  Vector2 pos;
  Rectangle area;
} Player;


typedef struct {
  Vector2 pos;
  Rectangle area;
  int stage;
  bool active;
} BunkerTile;

typedef struct {
  Vector2 pos;
  bool isFired;
  Rectangle area;
} Bullet;

typedef struct {
  Texture2D alienFrames[2];
  Rectangle alienArea;
  Vector2 pos;
  bool isAlive;
} Alien;


void draw_aliens(Alien aliens[3][ALIENS], bool up);
void draw_alien_row(Alien aliens[3][ALIENS],u8 row, bool up);
void reset_player_bullet(Player *player, Bullet *player_bullet,u8 bulletW);
void move_aliens(Alien aliens[3][ALIENS], i8 alien_velocity);
void init_bunker(BunkerTile bunker[BUNKER_ROWS][BUNKER_COLS], int startX, int startY, Texture2D tex);
void reset_alien_bullets(Bullet *alienBullets, Alien aliens[3][ALIENS], u8 alienW, u8 alienH);
int find_last_alien(int col, Alien aliens[3][ALIENS]);
void init_alien_bullets(Bullet alienBullets[ALIENS], u8 bulletW, u8 bulletH);
void init_aliens(Alien aliens[3][ALIENS], Texture2D alien_walks[3][2]);
void handle_alien_bullets(Bullet alienBullets[], BunkerTile bunkers[BUNKERS][BUNKER_ROWS][BUNKER_COLS]);
bool bullet_hits_tile(const Bullet *bullet, const BunkerTile *tile);
void damage_tile(BunkerTile *tile);
bool bullet_hits_bunkers(Bullet *bullet, BunkerTile bunkers[BUNKERS][BUNKER_ROWS][BUNKER_COLS]);
void handle_player_bullet(Bullet *player_bullet, Player *player, BunkerTile bunkers[BUNKERS][BUNKER_ROWS][BUNKER_COLS], int bulletW);
void display_initial_screen(u8 *lives, Font font_style, u8 height);
void player_bullet_hits_alien(Alien aliens[3][ALIENS], Player *player, Bullet *player_bullet, u16 *score, Sound alien_explosion,  u8 bulletW);
void alien_bullet_hits_player(Bullet alien_bullets[ALIENS], Rectangle *player_area, Sound player_death, u8 *lives, bool *is_playing);
void reload_alien_bullets(Bullet alien_bullets[ALIENS], Alien aliens[3][ALIENS], Sound fire_sound, u8 alienW, u8 alienH);
void move_alien_bullets(Bullet alien_bullets[ALIENS], u8 alien_bullet_speed, u8 i);
void move_player_left(Player *player, u16 player_speed, float dt);
void move_player_right(Player *player, u16 player_speed, float dt);
void fire_player_bullet(Bullet *player_bullet, u8 player_bullet_speed);


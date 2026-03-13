#include "./main.h"

int count_alive_aliens(Alien aliens[3][ALIENS]);
u16 find_leftmost_alien(Alien aliens[3][ALIENS]);
u16 find_rightmost_alien(Alien aliens[3][ALIENS], int alienWidth);

int main() {
  bool is_playing = false;
  bool up = false;
  u16 score = 0;
  u8 lives = 3;
  u8 alien_bullet_speed = 20;
  u8 player_bullet_speed = 8;
  u8 bullet_num = 0;
  u16 player_speed = 400;
  u16 bunker_gap = 500;
  u16 alien_speed = 8;
  i8 alienVelocityX = 10;
  float frame_time = 1.0f;
  float frame_limit = 2.0f;
  float alien_min_shoot_time = 0.0f;
  float alien_max_shoot_time = 2.0f;
  float alien_shoot_timer = 0.0f;
  float alien_shoot_delay = GetRandomValue(alien_min_shoot_time * 100, alien_max_shoot_time * 100) / 100.0f;

  InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Space Invaders");
  InitAudioDevice();
  SetTargetFPS(60);

  const Texture2D player_ship = LoadTexture("./assets/textures/PlayerShip.png");
  const Font font_style = LoadFont("./assets/Kenney High Square.ttf");
  const Texture2D bullet = LoadTexture("./assets/textures/PlayerMissile.png");
  const Texture2D alien1_walk1 = LoadTexture("./assets/textures/alien1_walk1.png");
  const Texture2D alien1_walk2 = LoadTexture("./assets/textures/alien1_walk2.png");
  const Texture2D alien2_walk1 = LoadTexture("./assets/textures/alien2_walk1.png");
  const Texture2D alien2_walk2 = LoadTexture("./assets/textures/alien2_walk2.png");
  const Texture2D alien3_walk1 = LoadTexture("./assets/textures/alien3_walk1.png");
  const Texture2D alien3_walk2 = LoadTexture("./assets/textures/alien3_walk2.png");
  const Texture2D logo = LoadTexture("./assets/textures/SpaceInvadersPixelArtLogo.png");
  const Texture2D bunker1 = LoadTexture("./assets/textures/BunkerPart0.png");
  const Texture2D bunker2 = LoadTexture("./assets/textures/BunkerPart1.png");
  const Texture2D bunker3 = LoadTexture("./assets/textures/BunkerPart2.png");
  const Sound alien_explosion = LoadSound("./assets/sounds/alien_explosion.wav");
  const Sound sound_per_frame = LoadSound("./assets/sounds/a.wav");
  const Sound fire_sound = LoadSound("./assets/sounds/missile_fire.wav");
  const Sound player_death = LoadSound("./assets/sounds/player_death.wav");

  Player player = {
    .pos = {
      SCREEN_WIDTH / 2.0f - player_ship.width / 2.0f,
      SCREEN_HEIGHT - 100,
    },
    .area = {
      SCREEN_WIDTH / 2.0f - player_ship.width / 2.0f,
      SCREEN_HEIGHT - 100,
      player_ship.width,
      player_ship.height
    }
  };

  Alien aliens[3][ALIENS];

  Bullet alien_bullets[ALIENS];

  Bullet player_bullet;

  Texture2D alien_walks[3][2] = {
    {alien1_walk1, alien1_walk2},
    {alien2_walk1, alien2_walk2},
    {alien3_walk1, alien3_walk2},
  };

  init_aliens(aliens, alien_walks);

  BunkerTile bunkers[BUNKERS][BUNKER_ROWS][BUNKER_COLS];

  init_bunker(bunkers[0], 200, SCREEN_HEIGHT - 200, bunker1);
  init_bunker(bunkers[1], 100 + bunker_gap, SCREEN_HEIGHT - 200, bunker1);
  init_bunker(bunkers[2], 100 + bunker_gap*2, SCREEN_HEIGHT - 200, bunker1);
  init_bunker(bunkers[3], 100 + bunker_gap*3, SCREEN_HEIGHT - 200, bunker1);


  init_alien_bullets(alien_bullets, bullet.width, bullet.height);

  reset_player_bullet(&player, &player_bullet, bullet.width);
  reset_alien_bullets(alien_bullets, aliens, alien1_walk1.width, alien1_walk1.height);

  while(!WindowShouldClose()){
    BeginDrawing();
    ClearBackground(BLACK);
    if(!is_playing) {
      DrawTextureEx(logo, (Vector2){SCREEN_WIDTH / 2.0f - logo.width / 2.0f, SCREEN_HEIGHT / 2.0f - logo.height / 2.0f}, 0, 1, WHITE);

      display_initial_screen(&lives, font_style, logo.height);

      if(IsKeyPressed(KEY_ENTER)) {
        is_playing = true;
      }
    }
    else {

      const float dt = GetFrameTime();

      frame_time += dt;
      alien_shoot_timer += dt;

      player_bullet_hits_alien(aliens, &player, &player_bullet, &score, alien_explosion, bullet.width);
      handle_player_bullet(&player_bullet, &player, bunkers, bullet.width);
      handle_alien_bullets(alien_bullets, bunkers);

      alien_bullet_hits_player(alien_bullets, &player.area, player_death, &lives, &is_playing);

      int alive = count_alive_aliens(aliens);

      if(alive < 32 && alive > 20)       frame_limit = 0.8f;
      else if(alive > 20)  frame_limit = 0.6f;
      else if(alive > 10)  frame_limit = 0.4f;
      else if(alive > 5)   frame_limit = 0.20f;
      else if(alive > 1)   frame_limit = 0.10f;
      else                 frame_limit = 0.01f;

      if(player_bullet.isFired) {
        if(player_bullet.pos.y + bullet.height < 0) {
          reset_player_bullet(&player, &player_bullet, bullet.width);
        }
        else {
          fire_player_bullet(&player_bullet, player_bullet_speed);
        }
      }

      for (u8 i = 0; i < ALIENS; i++) {
        if(alien_bullets[i].isFired) {
          move_alien_bullets(alien_bullets, alien_bullet_speed, i);
        }
        if(alien_bullets[i].pos.y > SCREEN_HEIGHT)
        {
          alien_bullets[i].isFired = false;
        }
      }

      if(alien_shoot_timer >= alien_shoot_delay) {
        reload_alien_bullets(alien_bullets, aliens, fire_sound, alien1_walk1.width, alien1_walk1.height);
        alien_shoot_timer = 0.0f;
        alien_shoot_delay = GetRandomValue(alien_min_shoot_time * 100, alien_max_shoot_time * 100) / 100.0f;
      }

      if(frame_time >= frame_limit){
        PlaySound(sound_per_frame);
        move_aliens(aliens, alienVelocityX);
        reset_alien_bullets(alien_bullets, aliens, alien1_walk1.width, alien2_walk1.height);
        up = !up;
        frame_time = 0.0f;

        int left = find_leftmost_alien(aliens);
        int right = find_rightmost_alien(aliens, alien1_walk1.width);

        if (left <= 0 || right >= SCREEN_WIDTH) {
            alienVelocityX *= -1;

            for (u8 row = 0; row < 3; row++) {
                for (u8 col = 0; col < ALIENS; col++) {
                    aliens[row][col].pos.y += 20;
                    aliens[row][col].alienArea.y = aliens[row][col].pos.y;
                }
            }
        }      
      }

      if(IsKeyDown(KEY_LEFT) || IsKeyDown(KEY_H) || IsKeyDown(KEY_A)) {
        if (player.pos.x >= 0) {
          move_player_left(&player, player_speed, dt);
          if(!player_bullet.isFired) {
            reset_player_bullet(&player, &player_bullet, bullet.width);
          }        
        }
      }

      if(IsKeyDown(KEY_RIGHT) || IsKeyDown(KEY_L) || IsKeyDown(KEY_D)) {
        if (player.pos.x + player.area.width <= SCREEN_WIDTH) {
          move_player_right(&player, player_speed, dt);

          if(!player_bullet.isFired) {
            reset_player_bullet(&player, &player_bullet, bullet.width);
          }
        }     
      }

      if(!player_bullet.isFired && IsKeyPressed(KEY_SPACE)){
        PlaySound(fire_sound);
        player_bullet.isFired = true;
      }


      DrawTextEx(font_style, TextFormat("%d", score), (Vector2){25,20}, 80, 5, WHITE);
      DrawTextEx(font_style, TextFormat("%d", lives), (Vector2){SCREEN_WIDTH - 60,20}, 80, 5, WHITE);

      DrawTextureEx(bullet,player_bullet.pos,0, 1, GREEN);

      for (int i = 0; i < ALIENS; i++) {
        if(alien_bullets[i].isFired) {
          DrawTextureEx(bullet, alien_bullets[i].pos, 0, 1, RED);
        }
      }

      draw_aliens(aliens, up);

      DrawTextureEx(player_ship,player.pos,0, 1, WHITE);

      for(int b = 0; b < BUNKERS; b++) {
        for(int row = 0; row < BUNKER_ROWS; row++) {
          for(int col = 0; col < BUNKER_COLS; col++) {
            BunkerTile *tile = &bunkers[b][row][col];

            if(!tile->active)
              continue;

            if(tile->stage == 0)
              DrawTextureEx(bunker1, tile->pos, 0, 1, WHITE);

            else if(tile->stage == 1)
              DrawTextureEx(bunker2, tile->pos, 0, 1, WHITE);

            else if(tile->stage == 2)
              DrawTextureEx(bunker3, tile->pos, 0, 1, WHITE);
          }      

        }
      }

    }
    EndDrawing();
  }

  CloseWindow();
  return 0;
}


void reset_player_bullet(Player *player, Bullet *player_bullet,u8 bulletW) {
  player_bullet->pos.x = player->area.x + (player->area.width / 2.0f) - bulletW + 1,
    player_bullet->pos.y = player->area.y + player->area.height / 2.0f + 5;
  player_bullet->area.x = player_bullet->pos.x;
  player_bullet->area.y = player_bullet->pos.y;
  player_bullet->isFired = false;
}


void draw_aliens(Alien aliens[3][ALIENS], bool up){
  for (u8 row = 0; row < 3; row++) {
    draw_alien_row(aliens, row, up);
  }
}

void draw_alien_row(Alien aliens[3][ALIENS],u8 row, bool up) {
  u8 frame;
  if(up) {
    frame = 1;
  }else {
    frame = 0;
  }
  for (u8 col = 0; col < ALIENS; col++) {
    if(aliens[row][col].isAlive) {
      const Texture2D currentFrame = aliens[row][col].alienFrames[frame];
      DrawTextureEx(currentFrame,aliens[row][col].pos, 0, 1, WHITE);
    }
  }
}

void move_aliens(Alien aliens[3][ALIENS], i8 alien_velocity) {
  for (u8 row = 0; row < 3; row++) {
    for (u8 col = 0; col < ALIENS; col++) {
      aliens[row][col].pos.x += alien_velocity;
      aliens[row][col].alienArea.x = aliens[row][col].pos.x;
    }
  }
}

void reset_alien_bullets(Bullet *alienBullets, Alien aliens[3][ALIENS], u8 alienW, u8 alienH) {
  for (int col = 0; col < ALIENS; col++) {
    if(!alienBullets[col].isFired) {
      int last_row = find_last_alien(col, aliens);

      if(last_row != -1) {
        alienBullets[col].pos.x = aliens[last_row][col].pos.x + alienW / 2.0f;
        alienBullets[col].pos.y = aliens[last_row][col].pos.y + alienH;

        alienBullets[col].area.x = alienBullets[col].pos.x;
        alienBullets[col].area.y = alienBullets[col].pos.y;
      }
    }
  }
}

int find_last_alien(int col, Alien aliens[3][ALIENS]) {
  int row = 2;
  while(row >= 0) {
    if(aliens[row][col].isAlive) {
      return row;
    }
    row--;
  }
  return -1;
}

void init_bunker(BunkerTile bunker[BUNKER_ROWS][BUNKER_COLS], int startX, int startY, Texture2D tex) {
  int x = startX;
  int y = startY;

  for (u8 row = 0; row < BUNKER_ROWS; row++) {
    for (u8 col = 0; col < BUNKER_COLS; col++) {
      bunker[row][col].pos = (Vector2){x,y};

      bunker[row][col].area = (Rectangle){
        x,
          y,
          tex.width,
          tex.height
      };

      bunker[row][col].stage = 0;
      bunker[row][col].active = true;

      x += tex.width;
    }

    y += tex.height;
    x = startX;
  }
}

void init_alien_bullets(Bullet *alienBullets, u8 bulletW, u8 bulletH) {
  for (int col = 0; col < ALIENS; col++) {
    alienBullets[col].area.width = bulletW;
    alienBullets[col].area.height = bulletH;
    alienBullets[col].isFired = false;
  }
}

void init_aliens(Alien aliens[3][ALIENS], Texture2D alien_walks[3][2]) {
  u16 posX = 100;
  u16 gapY = 50;
  for (u8 row = 0; row < 3; row++) {
    for (u8 col = 0; col < ALIENS; col++) {
      aliens[row][col].pos.x = posX + 100;
      aliens[row][col].pos.y = gapY;
      aliens[row][col].alienArea.x = aliens[row][col].pos.x;
      aliens[row][col].alienArea.y = aliens[row][col].pos.y;
      aliens[row][col].alienFrames[0] = alien_walks[row][0];
      aliens[row][col].alienFrames[1] = alien_walks[row][1];
      aliens[row][col].alienArea.width = alien_walks[0][0].width;
      aliens[row][col].alienArea.height = alien_walks[0][0].height;
      aliens[row][col].isAlive = true;
      posX += 100;
    }
    posX = 100;
    gapY += 100;
  }
}

bool bullet_hits_tile(const Bullet *bullet, const BunkerTile *tile) {
  return tile->active && CheckCollisionRecs(bullet->area, tile->area);
}

void damage_tile(BunkerTile *tile) {
  tile->stage++;

  if(tile->stage >= 3)
    tile->active = false;
}

bool bullet_hits_bunkers(Bullet *bullet, BunkerTile bunkers[BUNKERS][BUNKER_ROWS][BUNKER_COLS]) {
  for(int b = 0; b < BUNKERS; b++) {
    for(int row = 0; row < BUNKER_ROWS; row++){
      for(int col = 0; col < BUNKER_COLS; col++) {
        BunkerTile *tile = &bunkers[b][row][col];

        if(bullet_hits_tile(bullet, tile)) {
          damage_tile(tile);
          return true;
        }
      }
    }
  }

  return false;
}

void handle_player_bullet(Bullet *player_bullet, Player *player, BunkerTile bunkers[BUNKERS][BUNKER_ROWS][BUNKER_COLS], int bulletW) {
  if(!player_bullet->isFired)
    return;

  if(bullet_hits_bunkers(player_bullet, bunkers)) {
    reset_player_bullet(player, player_bullet, bulletW);
  }
}

void handle_alien_bullets(Bullet alienBullets[], BunkerTile bunkers[BUNKERS][BUNKER_ROWS][BUNKER_COLS]) {
  for(int i = 0; i < ALIENS; i++)
  {
    Bullet *bullet = &alienBullets[i];

    if(!bullet->isFired)
      continue;

    if(bullet_hits_bunkers(bullet, bunkers))
    {
      bullet->isFired = false;
      bullet->pos.y = -100;
    }
  }
}

void display_initial_screen(u8 *lives, Font font_style, u8 height) {
  const char *txt = "Press Enter to play";
  const int fontSize = 70;
  *lives = 3;
  Vector2 textSize = MeasureTextEx(font_style, txt, fontSize, 2);
  DrawTextEx(font_style, txt, (Vector2){SCREEN_WIDTH / 2.0f - textSize.x / 2.0f, SCREEN_HEIGHT / 2.0f + height}, fontSize, 2, GREEN);
}

void player_bullet_hits_alien(Alien aliens[3][ALIENS], Player *player, Bullet *player_bullet, u16 *score, Sound alien_explosion,  u8 bulletW) {
  for (u8 row = 0; row < 3; row++) {
    for (u8 col = 0; col < ALIENS; col++) {
      // DrawRectangleLinesEx(bullets[i].Area, 2, RED);
      // DrawRectangleLinesEx(aliens[row][col].alienArea, 2, GREEN);
      if(aliens[row][col].isAlive) {
        if(CheckCollisionRecs(aliens[row][col].alienArea, player_bullet->area)) {
          PlaySound(alien_explosion);
          reset_player_bullet(player, player_bullet, bulletW);
          aliens[row][col].isAlive = false;
          *score += 10;
        }
      }
    }
  }
}


void alien_bullet_hits_player(Bullet alien_bullets[ALIENS], Rectangle *player_area, Sound player_death, u8 *lives, bool *is_playing) {
  for (u8 i = 0; i < ALIENS; i++) {
    if(alien_bullets[i].isFired && CheckCollisionRecs(*player_area, alien_bullets[i].area)) {
      PlaySound(player_death);
      *lives = *lives - 1;
      alien_bullets[i].isFired = false;
      alien_bullets[i].pos.y = -100;
      if(*lives == 0) {
        *is_playing = false;
      }
    }
  }
}

void reload_alien_bullets(Bullet alien_bullets[ALIENS], Alien aliens[3][ALIENS], Sound fire_sound, u8 alienW, u8 alienH) {
    int attempts = ALIENS;

    while(attempts--) {
        int col = GetRandomValue(0, ALIENS - 1);
        int row = find_last_alien(col, aliens);

        if(row != -1) {
            PlaySound(fire_sound);

            alien_bullets[col].pos.x = aliens[row][col].pos.x + alienW / 2.0f;
            alien_bullets[col].pos.y = aliens[row][col].pos.y + alienH;

            alien_bullets[col].area.x = alien_bullets[col].pos.x;
            alien_bullets[col].area.y = alien_bullets[col].pos.y;

            alien_bullets[col].isFired = true;
            return;
        }
    }
}

void move_alien_bullets(Bullet alien_bullets[ALIENS], u8 alien_bullet_speed, u8 i) {
    alien_bullets[i].pos.y += alien_bullet_speed;
    alien_bullets[i].area.y = alien_bullets[i].pos.y;
}

void move_player_left(Player *player, u16 player_speed, float dt) {
  player->pos.x -= dt * player_speed;
  player->area.x = player->pos.x;
}

void move_player_right(Player *player, u16 player_speed, float dt) {
  player->pos.x += dt * player_speed;
  player->area.x = player->pos.x;
}

void fire_player_bullet(Bullet *player_bullet, u8 player_bullet_speed) {
    player_bullet->pos.y -= player_bullet_speed;
    player_bullet->area.y = player_bullet->pos.y;
}

int count_alive_aliens(Alien aliens[3][ALIENS]) {
    int alive = 0;

    for (u8 row = 0; row < 3; row++) {
        for (u8 col = 0; col < ALIENS; col++) {
            if (aliens[row][col].isAlive) {
                alive++;
            }
        }
    }
    return alive;
}

u16 find_leftmost_alien(Alien aliens[3][ALIENS]) {
    int minX = SCREEN_WIDTH;

    for (u8 row = 0; row < 3; row++) {
        for (u8 col = 0; col < ALIENS; col++) {
            if (aliens[row][col].isAlive) {
                if (aliens[row][col].pos.x < minX) {
                    minX = aliens[row][col].pos.x;
                }
            }
        }
    }

    return minX;
}

u16 find_rightmost_alien(Alien aliens[3][ALIENS], int alienWidth) {
    int maxX = 0;

    for (u8 row = 0; row < 3; row++) {
        for (u8 col = 0; col < ALIENS; col++) {
            if (aliens[row][col].isAlive) {
                int right = aliens[row][col].pos.x + alienWidth;

                if (right > maxX) {
                    maxX = right;
                }
            }
        }
    }

    return maxX;
}

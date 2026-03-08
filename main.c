#include "./main.h"

typedef struct {
  Vector2 pos;
  Rectangle area;
  int stage;
  bool active;
} BunkerTile;

void initBunker(BunkerTile bunker[2][5], int startX, int startY, Texture2D tex);
void resetAlienBullets(Bullet *alienBullets, Alien aliens[3][ALIENS], u8 alienW, u8 alienH);
int findLastAlien(int col, Alien aliens[3][ALIENS]);

int main() {
  u8 score = 0;
  u8 lives = 3;
  u8 bulletSpeed = 10;
  u8 total_bullets = 20;
  float frameTime = 1.0f;
  float frameLimit = 2.0f;
  u16 playerSpeed = 400;
  u16 alienSpeed = 8;
  u8 bulletNum = 0;
  bool isPlaying = false;
  bool up = false;
  float minShootTime = 0.5f;
  float maxShootTime = 3.0f;
  float shootTimer = 0.0f;
  float shootDelay = GetRandomValue(minShootTime * 100, maxShootTime * 100) / 100.0f;


  InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Space Invaders");
  InitAudioDevice();
  SetTargetFPS(60);

  const Texture2D player = LoadTexture("./assets/textures/PlayerShip.png");
  const Font fontStyle = LoadFont("./assets/Kenney High Square.ttf");
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

  Vector2 player_pos = {SCREEN_WIDTH / 2.0f - player.width / 2.0f, SCREEN_HEIGHT - 100};

  BunkerTile bunker1Pos[2][5];
  BunkerTile bunker2Pos[2][5];
  BunkerTile bunker3Pos[2][5];
  BunkerTile bunker4Pos[2][5];
  u16 bunkerGap = 500;

  initBunker(bunker1Pos, 100, SCREEN_HEIGHT - 200, bunker1);
  initBunker(bunker2Pos, 100 + bunkerGap, SCREEN_HEIGHT - 200, bunker1);
  initBunker(bunker3Pos, 100 + bunkerGap*2, SCREEN_HEIGHT - 200, bunker1);
  initBunker(bunker4Pos, 100 + bunkerGap*3, SCREEN_HEIGHT - 200, bunker1);

  Rectangle playerArea = {
    player_pos.x,
    player_pos.y,
    player.width,
    player.height
  };

  Alien aliens[3][ALIENS];

  i8 alienVelocityX = 10;

  u16 posX = 100;
  u16 gapY = 100;
  for (u8 row = 0; row < 3; row++) {
    for (u8 col = 0; col < ALIENS; col++) {
      aliens[row][col].pos.x = posX + 100;
      aliens[row][col].pos.y = gapY;
      aliens[row][col].alienArea.x = aliens[row][col].pos.x;
      aliens[row][col].alienArea.y = aliens[row][col].pos.y;
      posX += 100;
    }
    posX = 100;
    gapY += 100;
  }

  for (u8 col = 0; col < ALIENS; col++) {
    aliens[0][col].alienFrames[0] = alien1_walk1;
    aliens[0][col].alienFrames[1] = alien1_walk2;
    aliens[0][col].alienArea.width = alien1_walk1.width;
    aliens[0][col].alienArea.height = alien1_walk2.height;
    aliens[0][col].isAlive = true;

    aliens[1][col].alienFrames[0] = alien2_walk1;
    aliens[1][col].alienFrames[1] = alien2_walk2;
    aliens[1][col].alienArea.width = alien2_walk1.width;
    aliens[1][col].alienArea.height = alien2_walk1.height;
    aliens[1][col].isAlive = true;

    aliens[2][col].alienFrames[0] = alien3_walk1;
    aliens[2][col].alienFrames[1] = alien3_walk2;
    aliens[2][col].alienArea.width = alien3_walk1.width;
    aliens[2][col].alienArea.height = alien3_walk1.height;
    aliens[2][col].isAlive = true;
  }

  Bullet alienBullets[ALIENS];

  for (int row = 0; row < ALIENS; row++) {
    alienBullets[row].Area.width = bullet.width;
    alienBullets[row].Area.height = bullet.height;
    alienBullets[row].isFired = false;
  }


  Bullet bullets[total_bullets];

  resetBullets(bullets, playerArea,bullet.width, total_bullets, bullet.width,bullet.height);
  resetAlienBullets(alienBullets, aliens, alien1_walk1.width, alien1_walk1.height);

  while(!WindowShouldClose()){
    BeginDrawing();
    ClearBackground(BLACK);
    if(!isPlaying) {
      DrawTextureEx(logo, (Vector2){SCREEN_WIDTH / 2.0f - logo.width / 2.0f, SCREEN_HEIGHT / 2.0f - logo.height / 2.0f}, 0, 1, WHITE);

      const char *txt = "Press Enter to play";
      const int fontSize = 70;
      lives = 3;
      Vector2 textSize = MeasureTextEx(fontStyle, txt, fontSize, 2);
      DrawTextEx(fontStyle, txt, (Vector2){SCREEN_WIDTH / 2.0f - textSize.x / 2.0f, SCREEN_HEIGHT / 2.0f + logo.height}, fontSize, 2, GREEN);
      if(IsKeyPressed(KEY_ENTER)) {
        isPlaying = true;
      }
    }
    else {

      const float dt = GetFrameTime();
      frameTime += dt;
      shootTimer += dt;

      for (u8 i = 0; i < total_bullets; i++) {

        for(u8 row = 0; row < 2; row++)
        {
          for(u8 col = 0; col < 5; col++)
          {
            BunkerTile *tiles[4] = {
              &bunker1Pos[row][col],
              &bunker2Pos[row][col],
              &bunker3Pos[row][col],
              &bunker4Pos[row][col]
            };

            for(int b = 0; b < 4; b++)
            {
              BunkerTile *tile = tiles[b];

              if(tile->active && bullets[i].isFired &&
                  CheckCollisionRecs(bullets[i].Area, tile->area))
              {
                tile->stage++;

                if(tile->stage >= 3)
                  tile->active = false;

                resetBullet(bullets, playerArea, bullet.width, i);
                break;
              }
            }
          }
        }
      }
      for (u8 i = 0; i < total_bullets; i++) {
        for (u8 row = 0; row < 3; row++) {
          for (u8 col = 0; col < ALIENS; col++) {
            // DrawRectangleLinesEx(bullets[i].Area, 2, RED);
            // DrawRectangleLinesEx(aliens[row][col].alienArea, 2, GREEN);
            if(aliens[row][col].isAlive) {
              if(CheckCollisionRecs(aliens[row][col].alienArea, bullets[i].Area)) {
                PlaySound(alien_explosion);
                resetBullet(bullets, playerArea, bullet.width, i);
                aliens[row][col].isAlive = false;
                score += 10;
              }
            }
          }
        }
      }

      for (u8 i = 0; i < ALIENS; i++) {

        for(u8 row = 0; row < 2; row++)
        {
          for(u8 col = 0; col < 5; col++)
          {
            BunkerTile *tiles[4] = {
              &bunker1Pos[row][col],
              &bunker2Pos[row][col],
              &bunker3Pos[row][col],
              &bunker4Pos[row][col]
            };

            for(int b = 0; b < 4; b++)
            {
              BunkerTile *tile = tiles[b];

              if(tile->active &&
                  alienBullets[i].isFired &&
                  CheckCollisionRecs(alienBullets[i].Area, tile->area))
              {
                tile->stage++;

                if(tile->stage >= 3)
                  tile->active = false;

                alienBullets[i].isFired = false;
                break;
              }
            }
          }
        }
      }

      for (u8 i = 0; i < ALIENS; i++) {
        if(alienBullets[i].isFired && CheckCollisionRecs(playerArea, alienBullets[i].Area)) {
          PlaySound(player_death);
          lives--;
          alienBullets[i].isFired = false;
          alienBullets[i].pos.y = -100;
          if(lives == 0) {
            isPlaying = false;
          }
        }
      }

      if(shootTimer >= shootDelay) {
        PlaySound(fire_sound);
        u8 col = GetRandomValue(0, ALIENS - 1);
        i8 row = findLastAlien(col, aliens);


        if(row != -1)
        {
          alienBullets[col].pos.x = aliens[row][col].pos.x + alien1_walk1.width / 2.0f;
          alienBullets[col].pos.y = aliens[row][col].pos.y + alien1_walk1.height;

          alienBullets[col].Area.x = alienBullets[col].pos.x;
          alienBullets[col].Area.y = alienBullets[col].pos.y;

          alienBullets[col].isFired = true;
        }
        shootTimer = 0.0f;
        shootDelay = GetRandomValue(minShootTime * 100, maxShootTime * 100) / 100.0f;
      }

      if(frameTime >= frameLimit){
        PlaySound(sound_per_frame);
        moveAliens(aliens, alienVelocityX);
        resetAlienBullets(alienBullets, aliens, alien1_walk1.width, alien2_walk1.height);
        up = !up;
        frameTime = 0.0f;

        if (aliens[0][0].alienArea.x <= 0 || (aliens[0][ALIENS - 1].pos.x + alien1_walk1.width) >= SCREEN_WIDTH) {
          alienVelocityX *= -1.0f;
          for (u8 row = 0; row < 3; row++) {
            for (u8 col = 0; col < ALIENS; col++) {
              aliens[row][col].pos.y += 20;
              aliens[row][col].alienArea.y = aliens[row][col].pos.y;
            }
          }
        }
      }

      if(IsKeyDown(KEY_LEFT) || IsKeyDown(KEY_H) || IsKeyDown(KEY_A)) {
        if (player_pos.x >= 0) {
          player_pos.x -= dt * playerSpeed;
          playerArea.x = player_pos.x;

          for (u8 i = 0; i < total_bullets; i++) {
            if(!bullets[i].isFired) {
              bullets[i].pos.x = playerArea.x + (playerArea.width / 2.0f) - bullet.width + 1,
                bullets[i].pos.y = playerArea.y + playerArea.height/ 2.0f + 5;
              bullets[i].Area.x = bullets[i].pos.x;
              bullets[i].Area.y = bullets[i].pos.y;
            }        
          }
        }
      }

      if(IsKeyDown(KEY_RIGHT) || IsKeyDown(KEY_L) || IsKeyDown(KEY_D)) {
        if (player_pos.x + playerArea.width <= SCREEN_WIDTH) {
          player_pos.x += dt * playerSpeed;
          playerArea.x = player_pos.x;
          for (u8 i = 0; i < total_bullets; i++) {
            if(!bullets[i].isFired) {
              bullets[i].pos.x = playerArea.x + (playerArea.width / 2.0f) - bullet.width + 1,
                bullets[i].pos.y = playerArea.y + playerArea.height/ 2.0f + 5;
              bullets[i].Area.x = bullets[i].pos.x;
              bullets[i].Area.y = bullets[i].pos.y;
            }
          }     
        }
      }

      if(IsKeyPressed(KEY_SPACE)){
        PlaySound(fire_sound);
        bullets[bulletNum % total_bullets].isFired = true;
        bulletNum++;
      }


      DrawTextEx(fontStyle, TextFormat("%d", score), (Vector2){25,20}, 80, 5, WHITE);
      DrawTextEx(fontStyle, TextFormat("%d", lives), (Vector2){SCREEN_WIDTH - 60,20}, 80, 5, WHITE);

      for (u8 i = 0; i < total_bullets; i++) {
        DrawTextureEx(bullet,bullets[i].pos,0, 1, GREEN);
      }

      for (int i = 0; i < ALIENS; i++) {
        if(alienBullets[i].isFired) {
          DrawTextureEx(bullet, alienBullets[i].pos, 0, 1, RED);
        }
      }

      drawAliens(aliens, up);

      DrawTextureEx(player,player_pos,0, 1, WHITE);

      for (u8 row = 0; row < 2; row++) {
        for (u8 col = 0; col < 5; col++) {

          BunkerTile *tiles[4] = {
            &bunker1Pos[row][col],
            &bunker2Pos[row][col],
            &bunker3Pos[row][col],
            &bunker4Pos[row][col]
          };

          for(int b = 0; b < 4; b++)
          {
            BunkerTile *tile = tiles[b];

            if(!tile->active) continue;

            if(tile->stage == 0)
              DrawTextureEx(bunker1, tile->pos, 0, 1, WHITE);

            else if(tile->stage == 1)
              DrawTextureEx(bunker2, tile->pos, 0, 1, WHITE);

            else if(tile->stage == 2)
              DrawTextureEx(bunker3, tile->pos, 0, 1, WHITE);
          }
        }
      }
      for (u8 i = 0; i < total_bullets; i++) {
        if(bullets[i].isFired) {
          if(bullets[i].pos.y + bullet.height < 0) {
            resetBullet(bullets, playerArea, bullet.width, i);
          }
          else {
            bullets[i].pos.y -= bulletSpeed;
            bullets[i].Area.x = bullets[i].pos.x;
            bullets[i].Area.y = bullets[i].pos.y;
          }
        }
      }
      for (int i = 0; i < ALIENS; i++) {
        if(alienBullets[i].isFired) {
          alienBullets[i].pos.y += bulletSpeed;
          alienBullets[i].Area.x = alienBullets[i].pos.x;
          alienBullets[i].Area.y = alienBullets[i].pos.y;
        }
        if(alienBullets[i].pos.y > SCREEN_HEIGHT)
        {
          alienBullets[i].isFired = false;
        }
      }
    }
    EndDrawing();
  }

  CloseWindow();
  return 0;
}

void resetBullets(Bullet *bullets,Rectangle playerArea,u8 bulletWidth , u8 n, u8 width, u8 height) {
  for (u8 i = 0; i < n; i++) {
    resetBullet(bullets, playerArea, bulletWidth, i);
    bullets[i].Area.width = width;
    bullets[i].Area.height = height;
  }
}

void resetBullet(Bullet *bullets, Rectangle playerArea,u8 bulletWidth , u8 idx) {
  bullets[idx].pos.x = playerArea.x + (playerArea.width / 2.0f) - bulletWidth + 1,
    bullets[idx].pos.y = playerArea.y + playerArea.height/ 2.0f + 5;
  bullets[idx].isFired = false;
  bullets[idx].Area.x = bullets[idx].pos.x;
  bullets[idx].Area.y = bullets[idx].pos.y;
}


void drawAliens(Alien aliens[3][ALIENS], bool up){
  for (u8 row = 0; row < 3; row++) {
    drawAlienRow(aliens, row, up);
  }
}

void drawAlienRow(Alien aliens[3][ALIENS],u8 row, bool up) {
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

void moveAliens(Alien aliens[3][ALIENS], i8 alien_velocity) {
  for (u8 row = 0; row < 3; row++) {
    for (u8 col = 0; col < ALIENS; col++) {
      aliens[row][col].pos.x += alien_velocity;
      aliens[row][col].alienArea.x = aliens[row][col].pos.x;
    }
  }
}

void resetAlienBullets(Bullet *alienBullets, Alien aliens[3][ALIENS], u8 alienW, u8 alienH) {
  for (int col = 0; col < ALIENS; col++) {
    if(!alienBullets[col].isFired) {
      int last_row = findLastAlien(col, aliens);

      if(last_row != -1) {
        alienBullets[col].pos.x = aliens[last_row][col].pos.x + alienW / 2.0f;
        alienBullets[col].pos.y = aliens[last_row][col].pos.y + alienH;

        alienBullets[col].Area.x = alienBullets[col].pos.x;
        alienBullets[col].Area.y = alienBullets[col].pos.y;
      }
    }
  }
}
int findLastAlien(int col, Alien aliens[3][ALIENS]) {
  int row = 2;
  while(row >= 0) {
    if(aliens[row][col].isAlive) {
      return row;
    }
    row--;
  }
  return -1;
}

void initBunker(BunkerTile bunker[2][5], int startX, int startY, Texture2D tex)
{
  int x = startX;
  int y = startY;

  for (u8 row = 0; row < 2; row++)
  {
    for (u8 col = 0; col < 5; col++)
    {
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

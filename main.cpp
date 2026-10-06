/**
 * Author: Shahzaib
 * Assignment: Simple 2D Scene
 * Date due: 10/05/2026
 *
 * I pledge that I have completed this assignment without
 * collaborating with anyone else, in conformance with the
 * NYU School of Engineering Policies and Procedures on
 * Academic Misconduct.
 */

#include "CS3113/cs3113.h"
#include <math.h>

// Scenes
enum SceneState {APPROACH, IMPACT, CAPTURE, SUCCESS, RESET};
AppStatus gAppStatus = RUNNING;
SceneState gSceneState = APPROACH;

// Screen
constexpr int SCREEN_WIDTH  = 1280,
              SCREEN_HEIGHT = 720,
              FPS           = 60;

// Background
constexpr char BACKGROUND_FP[]  = "assets/skypillar.png", // created this background myself on Adobe Illustrator
               SKY_LIGHT[]  = "#87CEEB",
               SKY_MEDIUM[] = "#4FA3E3",
               SKY_DARK[]   = "#246BB2";
Texture2D gBackgroundTexture;

// Rayquaza
constexpr char RAYQUAZA_FP[]    = "assets/rayquaza.png"; // source: https://pokestop.io/pokemon/rayquaza
constexpr Vector2 RAYQUAZA_ORIGIN = {400.0f, 360.0f},
                  RAYQUAZA_SIZE = {352.0f, 352.0f};
constexpr float RAYQUAZA_X_RADIUS = 144.0f,
                RAYQUAZA_Y_RADIUS = 72.0f,
                RAYQUAZA_SPEED    = 1.5f;
Vector2 gRayquazaPosition = RAYQUAZA_ORIGIN,
        gRayquazaScale = RAYQUAZA_SIZE;
Texture2D gRayquazaTexture;

// Master Ball
constexpr char MASTER_BALL_FP[] = "assets/masterball.png"; // source: https://www.serebii.net/itemdex/masterball.shtml
constexpr Vector2 BALL_ORIGIN     = {1120.0f, 480.0f},
                  BALL_SIZE       = {128.0f, 128.0f};
constexpr float BALL_SPEED          = 160.0f,
                BALL_ROTATION_SPEED = 180.0f,
                CAPTURE_SPEED       = 2.0f;
Vector2 gBallPosition     = BALL_ORIGIN,
        gBallScale     = BALL_SIZE;
Texture2D gBallTexture;

// Star
constexpr char STAR_FP[]        = "assets/star.png"; // source: https://www.freeiconspng.com/img/616
constexpr Vector2 STAR_SIZE     = {64.0f, 64.0f};
constexpr float STAR_RISE_SPEED = 128.0f,
                STAR_WAVE_SPEED = 5.0f, 
                STAR_WAVE_WIDTH = 56.0f;
Vector2 gStarPosition     = BALL_ORIGIN,
        gStarScale     = STAR_SIZE;
Texture2D gStarTexture;

// Related to Delta Time
float gBallAngle = 0.0f;
float gPreviousTicks = 0.0f;
float gAnimationTime = 0.0f;
float gStateTime     = 0.0f;
float gStarTime      = 0.0f;

// Forward Declarations
void initialise();
void processInput();
void update();
void render();
void shutdown();

void resetScene();
void drawObject(Texture2D texture, Vector2 position, Vector2 scale, float angle);

// initialize
void initialise()
{
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Simple 2D Scene");
    gBackgroundTexture = LoadTexture(BACKGROUND_FP);
    gRayquazaTexture   = LoadTexture(RAYQUAZA_FP);
    gBallTexture       = LoadTexture(MASTER_BALL_FP);
    gStarTexture       = LoadTexture(STAR_FP);

    SetTargetFPS(FPS);
}

// processInput
void processInput()
{
    if (WindowShouldClose())
        gAppStatus = TERMINATED;
}

// update
void update()
{
    float ticks = static_cast<float>(GetTime());
    float deltaTime = ticks - gPreviousTicks;
    gPreviousTicks = ticks;

    gAnimationTime += deltaTime;
    gStateTime     += deltaTime;

    if (gSceneState == APPROACH)
    {
        gBallAngle -= BALL_ROTATION_SPEED * deltaTime;

        gRayquazaPosition.x = RAYQUAZA_ORIGIN.x + cos(gAnimationTime * RAYQUAZA_SPEED) * RAYQUAZA_X_RADIUS;
        gRayquazaPosition.y = RAYQUAZA_ORIGIN.y + sin(gAnimationTime * RAYQUAZA_SPEED * 2.0f) * RAYQUAZA_Y_RADIUS;

        gBallPosition.x -= BALL_SPEED * deltaTime;
        gBallPosition.y = BALL_ORIGIN.y + sin(gAnimationTime * 2.0f) * 64.0f;

        if (gBallPosition.x <= gRayquazaPosition.x)
        {
            gSceneState = IMPACT;
            gStateTime = 0.0f;
        }
    }

    else if (gSceneState == IMPACT)
    {
        gBallAngle += BALL_ROTATION_SPEED * deltaTime;

        if (gStateTime < 0.25f) gBallPosition.y -= 160.0f * deltaTime;

        else if (gStateTime < 0.50f) gBallPosition.y += 160.0f * deltaTime;

        else 
        {
            gSceneState = CAPTURE;
            gStateTime = 0.0f;
        }
    }

    else if (gSceneState == CAPTURE)
    {
        gBallAngle += BALL_ROTATION_SPEED * deltaTime;

        float distanceX = gBallPosition.x - gRayquazaPosition.x;
        float distanceY = gBallPosition.y - gRayquazaPosition.y;

        gRayquazaPosition.x += distanceX * CAPTURE_SPEED * deltaTime;
        gRayquazaPosition.y += distanceY * CAPTURE_SPEED * deltaTime;

        gRayquazaScale.x -= 160.0f * deltaTime;
        gRayquazaScale.y -= 160.0f * deltaTime;

        if (gRayquazaScale.x < 0.0f) gRayquazaScale.x = 0.0f;

        if (gRayquazaScale.y < 0.0f) gRayquazaScale.y = 0.0f;
        
        if (gRayquazaScale.x <= 0.0f)
        {
            gSceneState = SUCCESS;

            gStateTime = 0.0f;
            gStarTime  = 0.0f;

            gStarPosition = gBallPosition;
        }
    }

    else if (gSceneState == SUCCESS)
    {
        gStarTime += deltaTime;

        gStarPosition.x = gBallPosition.x + sin(gStarTime * STAR_WAVE_SPEED) * STAR_WAVE_WIDTH;
        gStarPosition.y = gBallPosition.y - gStarTime * STAR_RISE_SPEED;

        if (gStateTime >= 4.0f) gSceneState = RESET;
    }

    else if (gSceneState == RESET) resetScene();
}

// resetScene: I made this so that the animation starts over after reaching the final phase
void resetScene()
{
    gRayquazaPosition = RAYQUAZA_ORIGIN;
    gBallPosition     = BALL_ORIGIN;
    gStarPosition     = BALL_ORIGIN;

    gRayquazaScale = RAYQUAZA_SIZE;
    gBallScale     = BALL_SIZE;
    gStarScale     = STAR_SIZE;

    gBallAngle = 0.0f;

    gAnimationTime = 0.0f;
    gStateTime     = 0.0f;
    gStarTime      = 0.0f;

    gSceneState = APPROACH;
}

// drawObject: It's a helper to make textures for me
void drawObject(Texture2D texture, Vector2 position, Vector2 scale, float angle)
{
    Rectangle textureArea = {0.0f, 0.0f, static_cast<float>(texture.width), static_cast<float>(texture.height)};

    Rectangle destinationArea = {position.x, position.y, scale.x, scale.y};

    Vector2 objectOrigin = {scale.x / 2.0f, scale.y / 2.0f};

    DrawTexturePro(texture, textureArea, destinationArea, objectOrigin, angle, WHITE);
}

// render
void render()
{
    BeginDrawing();

    Color backgroundColour;

    float backgroundTime = fmod(gAnimationTime, 6.0f);

    if (backgroundTime < 2.0f) backgroundColour = ColorFromHex(SKY_LIGHT);

    else if (backgroundTime < 4.0f) backgroundColour = ColorFromHex(SKY_MEDIUM);

    else backgroundColour = ColorFromHex(SKY_DARK);

    ClearBackground(backgroundColour);

    Rectangle backgroundArea = {0.0f, 0.0f, static_cast<float>(gBackgroundTexture.width), static_cast<float>(gBackgroundTexture.height)};
    Rectangle backgroundDestination = {0.0f, 0.0f, static_cast<float>(SCREEN_WIDTH), static_cast<float>(SCREEN_HEIGHT)};

    Vector2 backgroundOrigin = {0.0f,0.0f};

    DrawTexturePro(gBackgroundTexture, backgroundArea, backgroundDestination, backgroundOrigin, 0.0f, WHITE);

    if (gSceneState != SUCCESS && gSceneState != RESET) drawObject(gRayquazaTexture, gRayquazaPosition, gRayquazaScale, 0.0f);

    drawObject(gBallTexture, gBallPosition, gBallScale, gBallAngle);

    if (gSceneState == SUCCESS) drawObject(gStarTexture, gStarPosition, gStarScale, 0.0f);

    EndDrawing();
}

// shutdown
void shutdown()
{
    UnloadTexture(gBackgroundTexture);
    UnloadTexture(gRayquazaTexture);
    UnloadTexture(gBallTexture);
    UnloadTexture(gStarTexture);

    CloseWindow();
}

// main
int main(void)
{
    initialise();

    while (gAppStatus == RUNNING)
    {
        processInput();
        update();
        render();
    }

    shutdown();

    return 0;
}
#pragma once
#include <windows.h>
#include <stdio.h>

#define LOG(format, ...) log(__FILE__, __LINE__, format, __VA_ARGS__);

void log(const char file[], int line, const char* format, ...);

#define CAP(n) ((n <= 0.0f) ? n=0.0f : (n >= 1.0f) ? n=1.0f : n=n)

#define DEGTORAD 0.0174532925199432957f
#define RADTODEG 57.295779513082320876f

typedef unsigned int uint;

enum update_status
{
	UPDATE_CONTINUE = 1,
	UPDATE_STOP,
	UPDATE_ERROR
};

// Configuration -----------
#define SCREEN_WIDTH 700
#define SCREEN_HEIGHT 650
#define SCREEN_SIZE 1
#define WIN_FULLSCREEN false
#define WIN_RESIZABLE false
#define WIN_BORDERLESS false
#define WIN_FULLSCREEN_DESKTOP false
#define VSYNC true
#define TITLE "Super Amazing Pinball"

//Adjustments
#define GRAVITY_X 0.0f
#define GRAVITY_Y 6.0f

#define BALL_RADIUS 8

#define SAVE_BALL_TIMER 550

#define BALL_BOUNCE_SOUND_TIMER 10

#define LIGHT_ACTIVATION_SPACING 100

#define BUMPER_FORCE_X 20
#define BUMPER_FORCE_Y 50
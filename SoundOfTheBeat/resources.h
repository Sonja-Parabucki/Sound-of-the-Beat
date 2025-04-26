#pragma once
#include "aim.h"
#include "background.h"
#include "explosion.h"
#include "ball.h"
#include "bomb.h"
#include "lights.h"

struct Resources {
    Aim& aim;
    Background& background;
    BallModel& ballModel;
    BombModel& bombModel;
    Explosion& explosion;
    Lights& lights;
};
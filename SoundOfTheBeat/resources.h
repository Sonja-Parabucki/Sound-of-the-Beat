#pragma once
#include "aim.h"
#include "background.h"
#include "explosion.h"
#include "ball.h"

struct Resources {
    Aim& aim;
    Background& background;
    BallModel& ballModel;
    Explosion& explosion;
};
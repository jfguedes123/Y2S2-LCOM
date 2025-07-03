#pragma once

#define SCREEN_W 1024
#define SCREEN_H 768

#include "../controllers/i8042.h"
#include "../controllers/i8254.h"
#include "../devices/kbc/KBC.h"
#include "../devices/keyboard/keyboard.h"
#include "../devices/mouse/mouse.h"
#include "../devices/video/video.h"

#include "state.h"

#include "menu.h"
#include "play.h"
#include "rules.h"

#include <stdint.h>
#include <stdio.h>
#include <string.h>

//
//  PlayerPawnModel.cpp
//  CapriciousCroissants
//
//  Created by Brandon Walker on 3/11/18.
//  Copyright © 2018 Game Design Initiative at Cornell. All rights reserved.
//

#include "PlayerPawnModel.h"

PlayerPawnModel::PlayerPawnModel() :
x(0),
y(0),
dx(0),
dy(1){
}

void PlayerPawnModel::dispose() {
}

// Turn Around (inver dx & dy)
void PlayerPawnModel::turnAround() {
    dx = -dx;
    dy = -dy;
}

// Step Position
void PlayerPawnModel::step() {
    x += dx;
    y += dy;
}

/*
void setHealth(int health) {
	_health = health;
}

void changeHealth(int changeAmount){
	setHealth(_health + changeAmount);
}

void setMaxHealth(){
	setHealth(maxHealth);
}


*/

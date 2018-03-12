//
//  PlayerPawnModel.h
//  CapriciousCroissants
//
//  Created by Brandon Walker on 3/11/18.
//  Copyright © 2018 Game Design Initiative at Cornell. All rights reserved.
//

#ifndef __Player_Pawn_Model_H__
#define __Player_Pawn_Model_H__

#include <cugl/cugl.h>


class PlayerPawnModel {
private:
	//int _health;
	//int _maxHealth;

public:
    PlayerPawnModel();
    ~PlayerPawnModel() { dispose(); }
    void dispose();
	//int getHealth() const { return _health; }
	//void setHealth(int health);
	//void setMaxHealth();
	//void changeHealth( int changeAmount);
	int x;
	int y;
	//int getIndex();
    
    // Direction Pawn is facing (no diagonals)
    int dx;
    int dy;
    // Turn Around (inver dx & dy)
    void turnAround();
    // Step Position
    void step();
    // Face random direction
    void randomDirection();
};

#endif /* __Player_Pawn_Model_H__ */

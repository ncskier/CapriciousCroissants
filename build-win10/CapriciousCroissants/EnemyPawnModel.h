//
//  EnemyPawnModel.h
//  CapriciousCroissants
//
//  Created by Joe Ienna on 3/11/18.
//  Copyright � 2018 Game Design Initiative at Cornell. All rights reserved.
//

#ifndef __Enemy_Pawn_Model_H__
#define __Enemy_Pawn_Model_H__

#include <cugl/cugl.h>



#pragma once
class EnemyPawnModel
{
private:
	//int _health;
	//int _maxHealth;
	int movementSpeed;
	//int attackDamage;

public:
	//int getHealth() const { return _health; }
	//void setHealth(int health);
	//void setMaxHealth();
	//void changeHealth( int changeAmount);
	int x;
	int y;
	//int getIndex();
public:
	EnemyPawnModel();
	~EnemyPawnModel();
};


#endif /* __Enemy_Pawn_Model_H__ */

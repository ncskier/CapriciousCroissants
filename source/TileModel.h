//
//  TileModel.h
//  CapriciousCroissants
//
//  Created by Brandon Walker on 3/11/18.
//  Copyright © 2018 Game Design Initiative at Cornell. All rights reserved.
//

#ifndef __Tile_Model_H__
#define __Tile_Model_H__

#include <cugl/cugl.h>
class TileModel {
protected:
	int _sideSize;

private:
	int _index;
	cugl::Color4 _color;

public:
	int getIndex() const { return _index; }

	cugl::Color4 getColor() const { return _color;	}

	void setColor(cugl::Color4 c);

	int x;

	int y;

		


};

#endif /* __Tile_Model_H__ */

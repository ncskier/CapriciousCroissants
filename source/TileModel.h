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
    int _color;

private:
	int _index;

public:
    int getIndex() const { return _index; }

    int getColor() const { return _color; }

    void setColor(int color) { _color = color; }
};

#endif /* __Tile_Model_H__ */

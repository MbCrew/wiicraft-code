// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, version 3.0.

// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License 3.0 for more details.

// Copyright (C) 2012-2013	filfat, xerpi, JoostinOnline

#include "Block.hpp"

//Constructor
	Block::Block(Chunk *chunkP, int pos_x, int pos_y, int pos_z, int ID2){
		
		ID = ID2;
		transparent = ID2;
		chunkPointer = chunkP;	
		
	}
	
//Destructor
	Block::~Block(){
		
	}
	
//Methods
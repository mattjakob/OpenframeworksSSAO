/*
 *  Camera.h
 *  fluidMultiTouch
 *
 *  Created by Benjamin Blundell on 26/03/2010.
 *  Copyright 2010 __MyCompanyName__. All rights reserved.
 *
 */
/*
 This file is part of ScreenSpaceAmbientOcclusion.
 
 ScreenSpaceAmbientOcclusion is free software: you can redistribute it 
 and/or modify it under the terms of the GNU Lesser General Public License 
 as published by the Free Software Foundation, either version 3 of the License, 
 or (at your option) any later version.
 
 ScreenSpaceAmbientOcclusion is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU Lesser General Public License for more details.
 
 You should have received a copy of the GNU Lesser General Public License
 along with ScreenSpaceAmbientOcclusion.  
 If not, see <http://www.gnu.org/licenses/>.
 
					 __  .__              ________ 
 ______ ____   _____/  |_|__| ____   ____/   __   \
 /  ___// __ \_/ ___\   __\  |/  _ \ /    \____    /
 \___ \\  ___/\  \___|  | |  (  <_> )   |  \ /    / 
 /____  >\___  >\___  >__| |__|\____/|___|  //____/  .co.uk
 \/     \/     \/                    \/          
 
 
 THE GHOST IN THE CSH
 
 
 Author: Benjamin Blundell (oni)
 Email: oni@section9.co.uk
 Web: http://www.section9.co.uk
 Blog: http://blog.section9.co.uk
 
 
 /* Screen Buffer taken from this page: http://www.songho.ca/opengl/gl_fbo.html */
/* SSAO Shader 2 from this page http://www.gamerendering.com/2009/01/14/ssao/ */


#ifndef CAMERA_H
#define CAMERA_H


#include "ofMain.h"


/*
 Usage:
 You can access the Right/Up/Forward vectors like a float[3] array,
 but cannot write to them, because that would screw up the matrix.
 Same goes for Position, except its safe to write to it.
 
 setView() has to be called before you draw anything.
 Just call it instead of gluLookAt (which most are using)
 
 move and rotate come in two versions. Loc means the transformation
 is in local coords, so rotating around (1,0,0) means youre rotating
 around your current Right-vector while Glob would rotate around the
 global x-axis.
 
 Most likely you will use Loc for controlling the camera, though Glob
 can be usefull if you need to apply physics. Also walking characters
 will usually rotate around the global rather than around their local Up,
 while flying objects will always use local axes.
 
 If talking about objects when this is a camera confuses you: if you drop
 the setView() method you can use this for objects in your world too. Just
 rename the class to Object3D or something and derive a camera class from it.
 */

class Camera {
public:
	float const *Right, *Up, *Forward;
	float *Position;
private:
	float Transform[16];
	
public:
	Camera(float x=0.0f, float y=0.0f, float z=0.0f);
	~Camera();
	
	void setView();
	void moveLoc(float x, float y, float z, float distance=1);
	void moveGlob(float x, float y, float z, float distance=1);
	void rotateLoc(float deg, float x, float y, float z);
	void rotateGlob(float deg, float x, float y, float z);
};

#endif
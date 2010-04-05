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


#include "ofMain.h"
#include "testApp.h"
#include "ofAppGlutWindow.h"

//========================================================================
int main( ){

    ofAppGlutWindow window;
	ofSetupOpenGL(&window, 1280,720, OF_WINDOW);			// <-------- setup the GL context

	//ofSetupOpenGL(&window, 1024,768, OF_FULLSCREEN);
	
	// this kicks off the running of my app
	// can be OF_WINDOW or OF_FULLSCREEN
	// pass in width and height too:
	ofRunApp( new testApp());

}

#ifndef _TEST_APP
#define _TEST_APP

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
#include "Camera.h"
#include "objLoader.h"
#include "ofxShader.h"
#include "ofxSimpleGuiToo.h"
#include "ofxQtVideoSaver.h"

class testApp : public ofBaseApp{

	public:
		void setup();
		void update();
		void draw();

		void keyPressed  (int key);
		void keyReleased(int key);
		void mouseMoved(int x, int y );
		void mouseDragged(int x, int y, int button);
		void mousePressed(int x, int y, int button);
		void mouseReleased(int x, int y, int button);
		void windowResized(int w, int h);

	private:
	
		std::string getRenderbufferParameters(GLuint id);
		std::string getTextureParameters(GLuint id);
		std::string convertInternalFormatToString(GLenum format);
		void printFramebufferInfo();
		void generateFrameBuffers();
		void generateScreenSpaceFrameBuffers();
		void initGL();
		void loadModels();
		bool checkFramebufferStatus();
		void generateTextures();
	
		void drawCity();
	
		void setupGUI();
	
		bool fboUsed;
		bool blurStep;
		bool showGui;
	
		GLuint fboId;                       // ID of FBO
		GLuint fboIdFinal;
		GLuint colourTextureId;             // ID of texture
		GLuint depthTextureId;
		GLuint noiseTextureId;
		GLuint finalTextureId;
		GLuint rboId;                       // ID of Renderbuffer object
		GLuint rboIdFinal;  
	
		float nearDist;  // near / far clip plane
		float farDist;
		float screenFov;
		float aspect;
	
		// Camera Settings
	
		Camera *camera;
	
		bool cameraRotate;
	float cameraXPos;
		float cameraZPos;
		float cameraRotation; 
	
		objLoader *londonCityModel;
	
		ofImage	depthImage;
		ofImage	colourImage;
		ofImage	finalImage;
		ofTexture depthBufferTexture;
		ofTexture colourBufferTexture;
		ofTexture finalBufferTexture;
	
		ofImage		randomImage;
		
	
		GLvoid* depthBytes;
		GLvoid* colourBytes;
		GLvoid* finalBytes;

		// Shaders
		ofxShader	ssaoShader;
		ofxShader	passShader;
		ofxShader	nmShader;
		ofxShader	ssao2Shader;
		ofxShader	blurShader;
	
		//SSAO2 Shader Values
	
		float ssaoRad;
		float ssaoFalloff;
		float ssaoOffset;
		float ssaoStrength;
		float totStrength;
	
		//Video Recording
		ofxQtVideoSaver		saver;
		ofImage				videoBuffer;
		bool				doRecording;
};

#endif

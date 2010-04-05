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


#include "testApp.h"
#include "glInfo.h"


//--------------------------------------------------------------
void testApp::setup(){

	initGL();
	generateTextures();
	generateScreenSpaceFrameBuffers();
	printFramebufferInfo();
	loadModels();
	setupGUI();
	
	cameraXPos =0;
	cameraZPos = -100;
	cameraRotation = 0;
	
	camera = new Camera(cameraXPos,400,cameraZPos);
	fboUsed = true;
	blurStep = false;
	showGui = true;
	cameraRotate = false;
	doRecording = false;
	
#ifdef DEBUG
    glInfo glInfo;
    glInfo.getInfo();
    glInfo.printSelf();
#endif
	
	
	// Setup variable for Near and Far Camera ranges
	
	int w = ofGetWidth();
	int h = ofGetHeight();
	
	float halfFov, theTan, screenFov, aspect;
	screenFov               = 60.0f;
	float eyeX              = (float)w / 2.0;
	float eyeY              = (float)h / 2.0;
	halfFov                 = PI * screenFov / 360.0;
	theTan                  = tanf(halfFov);
	float dist              = eyeY / theTan;
	nearDist  = dist / 10.0;  // near / far clip plane
	farDist   = dist * 10.0;
	aspect                  = (float)w/(float)h;
	
	totStrength = 1.38;
	
	// Shaders for SSAO and Passing
	// NOTE! Settings for shaders can ONLY be set when they are set active!!!
	
	ssaoShader.loadShader("ssao");
	if (!ssaoShader.bLoaded){
		cout << "SSAO Shader not loaded correctly" << endl;
	}
		passShader.loadShader("pass");
	if (!passShader.bLoaded){
		cout << "PASS Shader not loaded correctly" << endl;
	}
	nmShader.loadShader("nm");
	if (!nmShader.bLoaded){
		cout << "NM Shader not loaded correctly" << endl;
	}
	ssao2Shader.loadShader("ssao2");
	if (!ssao2Shader.bLoaded){
		cout << "SSAO2 Shader not loaded correctly" << endl;
	}
	blurShader.loadShader("blur");
	if (!blurShader.bLoaded){
		cout << "Blur Shader not loaded correctly" << endl;
	}

	// Camera Recording Setup
	saver.listCodecs();
	saver.setCodecType(18);
	saver.setCodecQualityLevel(OF_QT_SAVER_CODEC_QUALITY_NORMAL);
	saver.setup(1280,720,"output.mov");
	
	ofSetFrameRate(30);
	
}

void testApp::setupGUI(){
	
	// Setup the GUI
	
	gui.addTitle("SSAO Settings");
	gui.addToggle("Blur Step", blurStep);
	gui.addSlider("totStrength",totStrength, 0.01,2.0);
	gui.addSlider("ssaoStrength",ssaoStrength, 0.01,2.0);
	gui.addSlider("ssaoOffset",ssaoOffset, 0.0,30.0);
	gui.addSlider("ssaoFalloff",ssaoFalloff, 0.0000001,0.00001);
	gui.addSlider("ssaoRad",ssaoRad, 0.0001,0.01);

	gui.addTitle("Camera Settings");
	gui.addToggle("Camera Rotate", cameraRotate);
	gui.loadFromXML();
	gui.show();
}


//--------------------------------------------------------------
void testApp::update(){
	int w = ofGetWidth();
	int h = ofGetHeight();
	
	float halfFov, theTan, screenFov, aspect;
	screenFov               = 60.0f;
	float eyeX              = (float)w / 2.0;
	float eyeY              = (float)h / 2.0;
	halfFov                 = PI * screenFov / 360.0;
	theTan                  = tanf(halfFov);
	float dist              = eyeY / theTan;
	nearDist  = dist / 10.0;  // near / far clip plane
	farDist   = dist * 10.0;
	aspect                  = (float)w/(float)h;
	
	
	if(cameraRotate){
		cameraRotation =0.1;
		camera->rotateLoc(cameraRotation, 0, 1, 0);
	}
	
	if (doRecording){
		videoBuffer.grabScreen(0, 0, 1280, 720);
		saver.addFrame(videoBuffer.getPixels(), 1.0f / ofGetFrameRate()); 
	}
}

//--------------------------------------------------------------
void testApp::draw(){
			
	if(fboUsed)
    {
        // set the rendering destination to FBO
        glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, fboId);
		nmShader.setShaderActive(true);
		glViewport(0, 0, ofGetWidth(), ofGetHeight());
		
        glClearColor(1, 1, 1, 1);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
        glPushMatrix();
       
        camera->setView();
		drawCity();
		glPopMatrix();
		nmShader.setShaderActive(false);
		
#ifdef DEBUG
	//	glReadPixels(0,0,ofGetWidth(),ofGetHeight(),GL_COLOR_ATTACHMENT0_EXT,GL_RGB8, colourBytes);
	//	colourImage.saveImage("ColourBuffer.png");	
		
	//	glReadPixels(0,0,ofGetWidth(),ofGetHeight(),GL_DEPTH_ATTACHMENT_EXT,GL_UNSIGNED_BYTE, depthBytes);
	//	depthImage.saveImage("DepthBuffer.png");
		
#endif
		
		
		// Switch to Final Render Buffer
        glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, fboIdFinal);
		
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        // trigger mipmaps generation explicitly
        // NOTE: If GL_GENERATE_MIPMAP is set to GL_TRUE, then glCopyTexSubImage2D()
        // triggers mipmap generation automatically. However, the texture attached
        // onto a FBO should generate mipmaps manually via glGenerateMipmapEXT().
    
		// back to normal viewport and projection matrix
		glViewport(0, 0, ofGetWidth(), ofGetHeight());
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		glOrtho(-ofGetWidth()/2,ofGetWidth()/2,-ofGetHeight()/2,ofGetHeight()/2,1,20);
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
		glEnable(GL_TEXTURE_2D);
		ssao2Shader.setShaderActive(true);
			
		glActiveTextureARB(GL_TEXTURE0_ARB);
		glGenerateMipmapEXT(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D,colourTextureId);
		
		glActiveTextureARB(GL_TEXTURE1_ARB);
		glBindTexture(GL_TEXTURE_2D,noiseTextureId);
		
		 // This is needed and fails without but I suspect is a slow down bottleneck!
		
		ssao2Shader.setUniformVariable1i("normalMap",0);
		ssao2Shader.setUniformVariable1i("rnm",1);
		ssao2Shader.setUniformVariable1f("totStrength", totStrength);
		ssao2Shader.setUniformVariable1f("strength", ssaoStrength);
		ssao2Shader.setUniformVariable1f("offset", ssaoOffset);
		ssao2Shader.setUniformVariable1f("falloff",ssaoFalloff );
		ssao2Shader.setUniformVariable1f("rad", ssaoRad);
		glScalef(0.25, 0.25, 0.25);
		glTranslated(0,0,-2);
		glColor3f(1.0, 1.0, 1.0);
		
		glBegin(GL_QUADS);
		glMultiTexCoord2f(GL_TEXTURE0_ARB, 0, 0); glMultiTexCoord2f(GL_TEXTURE1_ARB, 0, 0);	glVertex3f(-ofGetWidth()/2,-ofGetHeight()/2,0);
		glMultiTexCoord2f(GL_TEXTURE0_ARB, 1, 0); glMultiTexCoord2f(GL_TEXTURE1_ARB, 1, 0);	glVertex3f(ofGetWidth()/2,-ofGetHeight()/2,0);
		glMultiTexCoord2f(GL_TEXTURE0_ARB, 1, 1); glMultiTexCoord2f(GL_TEXTURE1_ARB, 1, 1);	glVertex3f(ofGetWidth()/2,ofGetHeight()/2,0);
		glMultiTexCoord2f(GL_TEXTURE0_ARB, 0, 1); glMultiTexCoord2f(GL_TEXTURE1_ARB, 0, 1);	glVertex3f(-ofGetWidth()/2,ofGetHeight()/2,0);
		glEnd();
		
		ssao2Shader.setShaderActive(false);
		

		
		// back to normal window-system-provided framebuffer
        glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0); // unbind

		glViewport(0, 0, ofGetWidth(), ofGetHeight());
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		glOrtho(-ofGetWidth()/2,ofGetWidth()/2,-ofGetHeight()/2,ofGetHeight()/2,1,20);
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
		
		if (blurStep){
			blurShader.setShaderActive(true);
			blurShader.setUniformVariable2f("ScaleU",1.0/ofGetWidth(),1.0/ofGetHeight());
			blurShader.setUniformVariable1i("textureSource",2);
		}
		
		
		glActiveTextureARB(GL_TEXTURE2_ARB);
		glBindTexture(GL_TEXTURE_2D,finalTextureId);
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
		
		glPushMatrix();
		glTranslated(0,0,-5);
		glColor3f(0.95, 0.97, 1.0);
		glBegin(GL_QUADS);
		
		glMultiTexCoord2f(GL_TEXTURE2_ARB, 0, 0);glVertex3f(-ofGetWidth()/2,-ofGetHeight()/2,0);
		glMultiTexCoord2f(GL_TEXTURE2_ARB, 1, 0);glVertex3f(ofGetWidth()/2,-ofGetHeight()/2,0);
		glMultiTexCoord2f(GL_TEXTURE2_ARB, 1, 1);glVertex3f(ofGetWidth()/2,ofGetHeight()/2,0);
		glMultiTexCoord2f(GL_TEXTURE2_ARB, 0, 1);glVertex3f(-ofGetWidth()/2,ofGetHeight()/2,0);
		
		glEnd();
		glPopMatrix();
		
		if(blurStep)
			blurShader.setShaderActive(false);
		
		glDisable(GL_TEXTURE_2D);
		
		glPushMatrix();
		glTranslated(-ofGetWidth() /2,ofGetHeight() /2,-2);
		glRotatef(180, 1.0, 0, 0);
			if (showGui) gui.draw();
		glPopMatrix();
		
    }
	else {
		glMatrixMode(GL_PROJECTION);
		glPushMatrix();
		glLoadIdentity();
		glOrtho(-ofGetWidth()/2,ofGetWidth()/2,-ofGetHeight()/2,ofGetHeight()/2,1,20);
		glMatrixMode(GL_MODELVIEW);
		glPushMatrix();
		glLoadIdentity();
		glTranslated(0,0,-1500);
		glColor3f(1.0, 0.0, 0.0);
		glBegin(GL_QUADS);

		glVertex3f(-ofGetWidth()/2,-ofGetHeight()/2,0);
		glVertex3f(ofGetWidth()/2,-ofGetHeight()/2,0);
		glVertex3f(ofGetWidth()/2,ofGetHeight()/2,0);
		glVertex3f(-ofGetWidth()/2,ofGetHeight()/2,0);
		
		glEnd();
		glPopMatrix();
	}

	// Flick back to standard projection and draw our QUAD
	
	
}

//--------------------------------------------------------------
void testApp::keyPressed(int key){
	switch (key){
		case ' ':
			showGui = !showGui;
			break;
		case 'j':
			doRecording = true;
			break;
		case 'k':
			doRecording = false;
			saver.finishMovie();
			break;
	}
}

	
//--------------------------------------------------------------
void testApp::keyReleased(int key){

}

//--------------------------------------------------------------
void testApp::mouseMoved(int x, int y ){

}

//--------------------------------------------------------------
void testApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void testApp::mousePressed(int x, int y, int button){

}

//--------------------------------------------------------------
void testApp::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void testApp::windowResized(int w, int h){

}

//--------------------------------------------------------------

void testApp::drawCity(){
	glCullFace(GL_BACK);
	glPushMatrix();
	glTranslatef(ofGetWidth()/2,ofGetHeight()/2,0);
	glScalef(0.9f,0.9f,0.9f);
	glColor3f(0.3f, 0.3f, 0.3f);
	glRotatef(-90.0f, 1.0, 0.0, 0.0);
	glBegin(GL_TRIANGLES);
	
	for(int i=0; i<londonCityModel->faceCount; i++)
	{
		obj_face *o = londonCityModel->faceList[i];
		for(int j=0; j<3; j++)
		{
			glNormal3f(londonCityModel->normalList[o->normal_index[j]]->e[0], 
					   londonCityModel->normalList[o->normal_index[j]]->e[1],
					   londonCityModel->normalList[o->normal_index[j]]->e[2]);
			
			
			glVertex3f(londonCityModel->vertexList[ o->vertex_index[j] ]->e[0],
					   londonCityModel->vertexList[ o->vertex_index[j] ]->e[1],
					   londonCityModel->vertexList[ o->vertex_index[j] ]->e[2]);
			
		}
	}
	glEnd();
	glPopMatrix();
	
}



void testApp::loadModels(){
	londonCityModel = new objLoader();
	londonCityModel->load("/Users/oni/Projects/OpenFrameworksMac/apps/examples/ScreenSpaceAmbientOcclusion/bin/data/londontest.obj");
	
#ifdef DEBUG		
	printf("Number of vertices: %i\n", londonCityModel->vertexCount);
	printf("Number of vertex normals: %i\n", londonCityModel->normalCount);
	printf("Number of texture coordinates: %i\n", londonCityModel->textureCount);
	printf("\n");	
#endif
	
}


void testApp::initGL()
{
    glShadeModel(GL_SMOOTH);                    // shading mathod: GL_SMOOTH or GL_FLAT
    glPixelStorei(GL_UNPACK_ALIGNMENT, 4);      // 4-byte pixel alignment
    glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
    glEnable(GL_DEPTH_TEST);
    glClearColor(1, 1, 1, 1);                   // background color
    glClearStencil(0);                          // clear stencil buffer	
	ofSetVerticalSync(true);
   
}


void testApp::generateTextures(){
	
	// Generate a Colour Texture Object
	
	// create a texture object
    glGenTextures(1, &colourTextureId);
    glBindTexture(GL_TEXTURE_2D, colourTextureId);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_GENERATE_MIPMAP, GL_TRUE); // automatic mipmap generation included in OpenGL v1.4
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, ofGetWidth(), ofGetHeight(), 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);
    glBindTexture(GL_TEXTURE_2D, 0);
	
	// Generate ofImages to hold Textures
		
	colourBufferTexture.allocate(ofGetWidth(), ofGetHeight(), GL_RGBA);
	colourImage.allocate(ofGetWidth(), ofGetHeight(), OF_IMAGE_COLOR_ALPHA);
	colourBytes = colourImage.getPixels();
	
	// Generate a Depth Buffer Texture
	
	glGenTextures(1, &depthTextureId);
    glBindTexture(GL_TEXTURE_2D, depthTextureId);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
 //   glTexParameteri(GL_TEXTURE_2D, GL_GENERATE_MIPMAP, GL_TRUE); // automatic mipmap generation included in OpenGL v1.4
    glTexImage2D( GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, ofGetWidth(), ofGetHeight(), 0, GL_DEPTH_COMPONENT, GL_UNSIGNED_BYTE, 0);
    glBindTexture(GL_TEXTURE_2D, 0);
	
	// Generate ofImages to hold Textures
	
	depthBufferTexture.allocate(ofGetWidth(), ofGetHeight(), GL_LUMINANCE);
	depthImage.allocate(ofGetWidth(), ofGetHeight(), OF_IMAGE_GRAYSCALE);
	depthBytes = depthImage.getPixels();
	
	if (randomImage.loadImage("noise.png")){
		
		glGenTextures(1, &noiseTextureId);
		glBindTexture(GL_TEXTURE_2D, noiseTextureId);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexImage2D( GL_TEXTURE_2D, 0, GL_RGB,randomImage.getWidth(), randomImage.getHeight(), 0, GL_RGB, GL_UNSIGNED_BYTE, randomImage.getPixels());
		glBindTexture(GL_TEXTURE_2D, 0);
		
	}
	else{
		cout << "Failed to Load Noise Image for SSAO Shader" << endl;
	}

	// Generate Final Texture to hold the Image we will paste correctly
	glGenTextures(1, &finalTextureId);
    glBindTexture(GL_TEXTURE_2D, finalTextureId);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_GENERATE_MIPMAP, GL_TRUE); // automatic mipmap generation included in OpenGL v1.4
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, ofGetWidth(), ofGetHeight(), 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);
    glBindTexture(GL_TEXTURE_2D, 0);
	
	finalBufferTexture.allocate(ofGetWidth(), ofGetHeight(), GL_RGBA);
	finalImage.allocate(ofGetWidth(), ofGetHeight(), OF_IMAGE_COLOR_ALPHA);
	finalBytes = finalImage.getPixels();
	
}


void testApp::generateFrameBuffers(){
	
	// create a framebuffer object, you need to delete them when program exits.
	glGenFramebuffersEXT(1, &fboId);
	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, fboId);

	// create a renderbuffer object to store depth info
	// NOTE: A depth renderable image should be attached the FBO for depth test.
	// If we don't attach a depth renderable image to the FBO, then
	// the rendering output will be corrupted because of missing depth test.
	// If you also need stencil test for your rendering, then you must
	// attach additional image to the stencil attachement point, too.
	glGenRenderbuffersEXT(1, &rboId);
	glBindRenderbufferEXT(GL_RENDERBUFFER_EXT, rboId);
	glRenderbufferStorageEXT(GL_RENDERBUFFER_EXT, GL_DEPTH_COMPONENT, ofGetWidth(), ofGetHeight());
	glBindRenderbufferEXT(GL_RENDERBUFFER_EXT, 0);

	// attach a texture to FBO color attachement point
	glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT, GL_TEXTURE_2D, colourTextureId, 0);

	// attach a renderbuffer to depth attachment point
	glFramebufferRenderbufferEXT(GL_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT_EXT, GL_RENDERBUFFER_EXT, rboId);

	//@ disable color buffer if you don't attach any color buffer image,
	//@ for example, rendering depth buffer only to a texture.
	//@ Otherwise, glCheckFramebufferStatusEXT will not be complete.
	//glDrawBuffer(GL_NONE);
	//glReadBuffer(GL_NONE);

	// check FBO status
	printFramebufferInfo();
	bool status = checkFramebufferStatus();
	if(!status)
		fboUsed = false;
}


void testApp::generateScreenSpaceFrameBuffers() {
#ifdef DEBUG
	cout << "Creating First Framebuffer" <<endl;
#endif
	
	glGenFramebuffersEXT(1, &fboId);
	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, fboId);
	glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT_EXT, GL_TEXTURE_2D, depthTextureId, 0);
	glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT, GL_TEXTURE_2D, colourTextureId, 0);
	
	
#ifdef DEBUG	
	printFramebufferInfo();
#endif
	
	bool status = checkFramebufferStatus();
	if(!status)
		fboUsed = false;
	
#ifdef DEBUG
	cout << "Creating Second Framebuffer" <<endl;
#endif
	
	glGenFramebuffersEXT(1, &fboIdFinal);
	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, fboIdFinal);
	glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT, GL_TEXTURE_2D, finalTextureId, 0);
	glGenRenderbuffersEXT(1, &rboIdFinal);
	glBindRenderbufferEXT(GL_RENDERBUFFER_EXT, rboIdFinal);
	glRenderbufferStorageEXT(GL_RENDERBUFFER_EXT, GL_DEPTH_COMPONENT, ofGetWidth(), ofGetHeight());
	glBindRenderbufferEXT(GL_RENDERBUFFER_EXT, 0);
	glFramebufferRenderbufferEXT(GL_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT_EXT, GL_RENDERBUFFER_EXT, rboIdFinal);
	
#ifdef DEBUG	
	printFramebufferInfo();
#endif
	
	status = checkFramebufferStatus();
	if(!status)
		fboUsed = false;
	
	
	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
	
}

///////////////////////////////////////////////////////////////////////////////
// print out the FBO infos
///////////////////////////////////////////////////////////////////////////////

void  testApp::printFramebufferInfo()
{
    cout << "\n***** FBO STATUS *****\n";
	
    // print max # of colorbuffers supported by FBO
    int colorBufferCount = 0;
    glGetIntegerv(GL_MAX_COLOR_ATTACHMENTS_EXT, &colorBufferCount);
    cout << "Max Number of Color Buffer Attachment Points: " << colorBufferCount << endl;
	
    int objectType;
    int objectId;
	
    // print info of the colorbuffer attachable image
    for(int i = 0; i < colorBufferCount; ++i)
    {
        glGetFramebufferAttachmentParameterivEXT(GL_FRAMEBUFFER_EXT,
                                                 GL_COLOR_ATTACHMENT0_EXT+i,
                                                 GL_FRAMEBUFFER_ATTACHMENT_OBJECT_TYPE_EXT,
                                                 &objectType);
        if(objectType != GL_NONE)
        {
            glGetFramebufferAttachmentParameterivEXT(GL_FRAMEBUFFER_EXT,
                                                     GL_COLOR_ATTACHMENT0_EXT+i,
                                                     GL_FRAMEBUFFER_ATTACHMENT_OBJECT_NAME_EXT,
                                                     &objectId);
			
            std::string formatName;
			
            cout << "Color Attachment " << i << ": ";
            if(objectType == GL_TEXTURE)
                cout << "GL_TEXTURE, " << getTextureParameters(objectId) << endl;
            else if(objectType == GL_RENDERBUFFER_EXT)
                cout << "GL_RENDERBUFFER_EXT, " << getRenderbufferParameters(objectId) << endl;
        }
    }
	
    // print info of the depthbuffer attachable image
    glGetFramebufferAttachmentParameterivEXT(GL_FRAMEBUFFER_EXT,
                                             GL_DEPTH_ATTACHMENT_EXT,
                                             GL_FRAMEBUFFER_ATTACHMENT_OBJECT_TYPE_EXT,
                                             &objectType);
    if(objectType != GL_NONE)
    {
        glGetFramebufferAttachmentParameterivEXT(GL_FRAMEBUFFER_EXT,
                                                 GL_DEPTH_ATTACHMENT_EXT,
                                                 GL_FRAMEBUFFER_ATTACHMENT_OBJECT_NAME_EXT,
                                                 &objectId);
		
        cout << "Depth Attachment: ";
        switch(objectType)
        {
			case GL_TEXTURE:
				cout << "GL_TEXTURE, " << getTextureParameters(objectId) << endl;
				break;
			case GL_RENDERBUFFER_EXT:
				cout << "GL_RENDERBUFFER_EXT, " << getRenderbufferParameters(objectId) << endl;
				break;
        }
    }
	
    // print info of the stencilbuffer attachable image
    glGetFramebufferAttachmentParameterivEXT(GL_FRAMEBUFFER_EXT,
                                             GL_STENCIL_ATTACHMENT_EXT,
                                             GL_FRAMEBUFFER_ATTACHMENT_OBJECT_TYPE_EXT,
                                             &objectType);
    if(objectType != GL_NONE)
    {
        glGetFramebufferAttachmentParameterivEXT(GL_FRAMEBUFFER_EXT,
                                                 GL_STENCIL_ATTACHMENT_EXT,
                                                 GL_FRAMEBUFFER_ATTACHMENT_OBJECT_NAME_EXT,
                                                 &objectId);
		
        cout << "Stencil Attachment: ";
        switch(objectType)
        {
			case GL_TEXTURE:
				cout << "GL_TEXTURE, " << getTextureParameters(objectId) << endl;
				break;
			case GL_RENDERBUFFER_EXT:
				cout << "GL_RENDERBUFFER_EXT, " << getRenderbufferParameters(objectId) << endl;
				break;
        }
    }
	
    cout << endl;
}

///////////////////////////////////////////////////////////////////////////////
// return texture parameters as string using glGetTexLevelParameteriv()
///////////////////////////////////////////////////////////////////////////////
std::string testApp::getTextureParameters(GLuint id)
{
    if(glIsTexture(id) == GL_FALSE)
        return "Not texture object";
	
    int width, height, format;
    std::string formatName;
    glBindTexture(GL_TEXTURE_2D, id);
    glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_WIDTH, &width);            // get texture width
    glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_HEIGHT, &height);          // get texture height
    glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_INTERNAL_FORMAT, &format); // get texture internal format
    glBindTexture(GL_TEXTURE_2D, 0);
	
    formatName = convertInternalFormatToString(format);
	
    std::stringstream ss;
    ss << width << "x" << height << ", " << formatName;
    return ss.str();
}



///////////////////////////////////////////////////////////////////////////////
// return renderbuffer parameters as string using glGetRenderbufferParameterivEXT
///////////////////////////////////////////////////////////////////////////////
std::string testApp::getRenderbufferParameters(GLuint id)
{
    if(glIsRenderbufferEXT(id) == GL_FALSE)
        return "Not Renderbuffer object";
	
    int width, height, format;
    std::string formatName;
    glBindRenderbufferEXT(GL_RENDERBUFFER_EXT, id);
    glGetRenderbufferParameterivEXT(GL_RENDERBUFFER_EXT, GL_RENDERBUFFER_WIDTH_EXT, &width);    // get renderbuffer width
    glGetRenderbufferParameterivEXT(GL_RENDERBUFFER_EXT, GL_RENDERBUFFER_HEIGHT_EXT, &height);  // get renderbuffer height
    glGetRenderbufferParameterivEXT(GL_RENDERBUFFER_EXT, GL_RENDERBUFFER_INTERNAL_FORMAT_EXT, &format); // get renderbuffer internal format
    glBindRenderbufferEXT(GL_RENDERBUFFER_EXT, 0);
	
    formatName = convertInternalFormatToString(format);
	
    std::stringstream ss;
    ss << width << "x" << height << ", " << formatName;
    return ss.str();
}



///////////////////////////////////////////////////////////////////////////////
// convert OpenGL internal format enum to string
///////////////////////////////////////////////////////////////////////////////
std::string testApp::convertInternalFormatToString(GLenum format)
{
    std::string formatName;
	
    switch(format)
    {
		case GL_STENCIL_INDEX:
			formatName = "GL_STENCIL_INDEX";
			break;
		case GL_DEPTH_COMPONENT:
			formatName = "GL_DEPTH_COMPONENT";
			break;
		case GL_ALPHA:
			formatName = "GL_ALPHA";
			break;
		case GL_RGB:
			formatName = "GL_RGB";
			break;
		case GL_RGBA:
			formatName = "GL_RGBA";
			break;
		case GL_LUMINANCE:
			formatName = "GL_LUMINANCE";
			break;
		case GL_LUMINANCE_ALPHA:
			formatName = "GL_LUMINANCE_ALPHA";
			break;
		case GL_ALPHA4:
			formatName = "GL_ALPHA4";
			break;
		case GL_ALPHA8:
			formatName = "GL_ALPHA8";
			break;
		case GL_ALPHA12:
			formatName = "GL_ALPHA12";
			break;
		case GL_ALPHA16:
			formatName = "GL_ALPHA16";
			break;
		case GL_LUMINANCE4:
			formatName = "GL_LUMINANCE4";
			break;
		case GL_LUMINANCE8:
			formatName = "GL_LUMINANCE8";
			break;
		case GL_LUMINANCE12:
			formatName = "GL_LUMINANCE12";
			break;
		case GL_LUMINANCE16:
			formatName = "GL_LUMINANCE16";
			break;
		case GL_LUMINANCE4_ALPHA4:
			formatName = "GL_LUMINANCE4_ALPHA4";
			break;
		case GL_LUMINANCE6_ALPHA2:
			formatName = "GL_LUMINANCE6_ALPHA2";
			break;
		case GL_LUMINANCE8_ALPHA8:
			formatName = "GL_LUMINANCE8_ALPHA8";
			break;
		case GL_LUMINANCE12_ALPHA4:
			formatName = "GL_LUMINANCE12_ALPHA4";
			break;
		case GL_LUMINANCE12_ALPHA12:
			formatName = "GL_LUMINANCE12_ALPHA12";
			break;
		case GL_LUMINANCE16_ALPHA16:
			formatName = "GL_LUMINANCE16_ALPHA16";
			break;
		case GL_INTENSITY:
			formatName = "GL_INTENSITY";
			break;
		case GL_INTENSITY4:
			formatName = "GL_INTENSITY4";
			break;
		case GL_INTENSITY8:
			formatName = "GL_INTENSITY8";
			break;
		case GL_INTENSITY12:
			formatName = "GL_INTENSITY12";
			break;
		case GL_INTENSITY16:
			formatName = "GL_INTENSITY16";
			break;
		case GL_R3_G3_B2:
			formatName = "GL_R3_G3_B2";
			break;
		case GL_RGB4:
			formatName = "GL_RGB4";
			break;
		case GL_RGB5:
			formatName = "GL_RGB4";
			break;
		case GL_RGB8:
			formatName = "GL_RGB8";
			break;
		case GL_RGB10:
			formatName = "GL_RGB10";
			break;
		case GL_RGB12:
			formatName = "GL_RGB12";
			break;
		case GL_RGB16:
			formatName = "GL_RGB16";
			break;
		case GL_RGBA2:
			formatName = "GL_RGBA2";
			break;
		case GL_RGBA4:
			formatName = "GL_RGBA4";
			break;
		case GL_RGB5_A1:
			formatName = "GL_RGB5_A1";
			break;
		case GL_RGBA8:
			formatName = "GL_RGBA8";
			break;
		case GL_RGB10_A2:
			formatName = "GL_RGB10_A2";
			break;
		case GL_RGBA12:
			formatName = "GL_RGBA12";
			break;
		case GL_RGBA16:
			formatName = "GL_RGBA16";
			break;
		default:
			formatName = "Unknown Format";
    }
	
    return formatName;
}


///////////////////////////////////////////////////////////////////////////////
// check FBO completeness
///////////////////////////////////////////////////////////////////////////////
bool testApp::checkFramebufferStatus()
{
    // check FBO status
    GLenum status = glCheckFramebufferStatusEXT(GL_FRAMEBUFFER_EXT);
    switch(status)
    {
		case GL_FRAMEBUFFER_COMPLETE_EXT:
			std::cout << "Framebuffer complete." << std::endl;
			return true;
			
		case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT_EXT:
			std::cout << "[ERROR] Framebuffer incomplete: Attachment is NOT complete." << std::endl;
			return false;
			
		case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT_EXT:
			std::cout << "[ERROR] Framebuffer incomplete: No image is attached to FBO." << std::endl;
			return false;
			
		case GL_FRAMEBUFFER_INCOMPLETE_DIMENSIONS_EXT:
			std::cout << "[ERROR] Framebuffer incomplete: Attached images have different dimensions." << std::endl;
			return false;
			
		case GL_FRAMEBUFFER_INCOMPLETE_FORMATS_EXT:
			std::cout << "[ERROR] Framebuffer incomplete: Color attached images have different internal formats." << std::endl;
			return false;
			
		case GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER_EXT:
			std::cout << "[ERROR] Framebuffer incomplete: Draw buffer." << std::endl;
			return false;
			
		case GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER_EXT:
			std::cout << "[ERROR] Framebuffer incomplete: Read buffer." << std::endl;
			return false;
			
		case GL_FRAMEBUFFER_UNSUPPORTED_EXT:
			std::cout << "[ERROR] Unsupported by FBO implementation." << std::endl;
			return false;
			
		default:
			std::cout << "[ERROR] Unknow error." << std::endl;
			return false;
    }
}


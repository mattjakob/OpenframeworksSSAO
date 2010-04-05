/*This file is part of OnlyTribute.
 
 OnlyTribute is free software: you can redistribute it and/or modify
 it under the terms of the GNU Lesser General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.
 
 OnlyTribute is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU Lesser General Public License for more details.
 
 You should have received a copy of the GNU Lesser General Public License
 along with OnlyTribute.  If not, see <http://www.gnu.org/licenses/>.
 
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
 
 */


#ifndef _QT_VIDEO_GRABBER_
#define _QT_VIDEO_GRABBER_

#include "ofConstants.h"
#include "ofQtUtils.h"
#include "ofUtils.h"

#define OF_QT_SAVER_CODEC_QUALITY_LOSSLESS		0x00000400
#define OF_QT_SAVER_CODEC_QUALITY_MAX			0x000003FF
#define OF_QT_SAVER_CODEC_QUALITY_HIGH			0x00000300
#define OF_QT_SAVER_CODEC_QUALITY_NORMAL		0x00000200
#define OF_QT_SAVER_CODEC_QUALITY_LOW			0x00000100
#define OF_QT_SAVER_CODEC_QUALITY_MIN			0x00000000

class ofxQtVideoSaver{

	public :
								ofxQtVideoSaver			();
		void					setup					(int width , int height , string fileName );
		void					finishMovie				();
		void					addFrame				(unsigned char* data, float frameLengthInSecs = (1/30.0f));
		void					listCodecs				();
		void					setCodecType			(int chosenCodec );
		void					setGworldPixel			(GWorldPtr gwPtr, int r, int g, int b, short x, short y);  // just for checking...
		
		void					setCodecQualityLevel			(int level);		// see the #defines above...
		
		bool					bAmSetupForRecording	() { return bSetupForRecordingMovie; }
		void					addAudioTrack(string audioPath);
	private:
		
		//-------------------------------- movie
		
		bool					bSetupForRecordingMovie;
		
		
		int						w; 
		int						h;
	    int						codecQualityLevel;
		OSErr 					osErr;
	    short 					sResId;
	    short 					sResRefNum;
	    FSSpec 					fsSpec;
	    Movie 					movie;
	    Track 					track;
	    Media 					media;
	    string					fileName;
	    FSRef					fsref;

		// ------------------------------- frame
		Rect 					rect;
		RGBColor 				rgbColor;
		CodecType 				codecType;
		GWorldPtr 				pMovieGWorld;
		PixMapHandle 			pixMapHandle;
		Ptr 					pCompressedData;
		CGrafPtr 				pSavedPort;
		GDHandle 				hSavedDevice;
		Handle 					hCompressedData;
		long 					lMaxCompressionSize;
		ImageDescriptionHandle	hImageDescription;
};

#endif

/**
 * projectM -- Milkdrop-esque visualisation SDK
 * Copyright (C)2003-2004 projectM Team
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 * See 'LICENSE.txt' included within this release
 *
 */
/**
 * $Id: FBO.c,v 1.1.1.1 2005/12/23 18:05:00 psperl Exp $
 *
 * Render this methods
 */

#include <stdio.h>
//#include <GL/gl.h>

#include "Common.hpp"
#include "FBO.hpp"

#ifdef MACOS
#include <agl.h>
#endif /** MACOS */

RenderTarget::~RenderTarget() {


	glDeleteTextures( 1, &this->textureID[0]);

	if (useFBO) 
         {
		glDeleteTextures( 1, &this->textureID[1] );
		glDeleteRenderbuffersEXT(1,  &this->depthb[0]);
		glDeleteFramebuffersEXT(1, &this->fbuffer[0]);
		if(renderToTexture)
		  {
		    glDeleteTextures( 1, &this->textureID[2] );
		    glDeleteRenderbuffersEXT(1,  &this->depthb[1]);
		    glDeleteFramebuffersEXT(1, &this->fbuffer[1]);
		  }
         }

}

GLuint RenderTarget::initRenderToTexture()
{
  if (this->useFBO==1)
    {
      this->renderToTexture=1;
      
      GLuint   fb2, depth_rb2;
      glGenFramebuffersEXT(1, &fb2);
      glBindFramebufferEXT( GL_FRAMEBUFFER_EXT, fb2 );
      glGenRenderbuffersEXT(1, &depth_rb2);
      glBindRenderbufferEXT( GL_RENDERBUFFER_EXT, depth_rb2 );
      
      glRenderbufferStorageEXT( GL_RENDERBUFFER_EXT, GL_DEPTH_COMPONENT24, this->texsize,this->texsize  );
      glFramebufferRenderbufferEXT( GL_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT_EXT, GL_RENDERBUFFER_EXT, depth_rb2 );         
      this->fbuffer[1] = fb2;
      this->depthb[1]=  depth_rb2;
      glGenTextures(1, &this->textureID[2]);
      glBindTexture(GL_TEXTURE_2D, this->textureID[2]); 
      glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
      glTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA8, texsize, texsize, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL );
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
      glFramebufferTexture2DEXT( GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT, GL_TEXTURE_2D, this->textureID[2], 0 );
      return this->textureID[2];
    }
  else return -1;
      
}

/** Creates new pbuffers */
RenderTarget::RenderTarget(int texsize, int width, int height) : useFBO(false) {

    int mindim = 0;
    int origtexsize = 0;
    this->useFBO = true;
    this->renderToTexture = 0;

    this->texsize = texsize;


   if(this->useFBO)
    { 
      glewInit();
      
      if(glewIsSupported("GL_EXT_framebuffer_object"))
	{	 

	  GLuint   fb,  depth_rb, rgba_tex,  other_tex;
	  glGenFramebuffersEXT(1, &fb);
	  glBindFramebufferEXT( GL_FRAMEBUFFER_EXT, fb );
	  
	  glGenRenderbuffersEXT(1, &depth_rb);
	  glBindRenderbufferEXT( GL_RENDERBUFFER_EXT, depth_rb );
	  glRenderbufferStorageEXT( GL_RENDERBUFFER_EXT, GL_DEPTH_COMPONENT, this->texsize,this->texsize  );
	  glFramebufferRenderbufferEXT( GL_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT_EXT, GL_RENDERBUFFER_EXT, depth_rb );
	  this->fbuffer[0] = fb;
	  this->depthb[0]=  depth_rb;
	  
	  glGenTextures(1, &other_tex);
	  glBindTexture(GL_TEXTURE_2D,other_tex);
	  glTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA, texsize, texsize, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL );
	  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	  //glGenerateMipmapEXT(GL_TEXTURE_2D);
	  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
	  
	  
	  
	  glGenTextures(1, &rgba_tex);
	  glBindTexture(GL_TEXTURE_2D, rgba_tex); 
	  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	  glTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA, texsize, texsize, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL );
	  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	  //glGenerateMipmapEXT(GL_TEXTURE_2D);
	  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
	  
	  
	  
	  glFramebufferTexture2DEXT( GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT, GL_TEXTURE_2D, rgba_tex, 0 );         
	  this->textureID[0] = rgba_tex;
	  this->textureID[1] = other_tex; 
	  
	  GLenum status = glCheckFramebufferStatusEXT(GL_FRAMEBUFFER_EXT);
	  if (status == GL_FRAMEBUFFER_COMPLETE_EXT) {
	    return;
	  }	
	}
      else this->useFBO=0;      
    }


    DWRITE( "using teximage hack fallback\n" );

    /** Fallback pbuf;fer creation via teximage hack */
    /** Check the texture size against the viewport size */
    /** If the viewport is smaller, then we'll need to scale the texture size down */
    /** If the viewport is larger, scale it up */
    mindim = width < height ? width : height;
    origtexsize = this->texsize;
    this->texsize = nearestPower2( mindim, SCALE_MINIFY );      

    /* Create the texture that will be bound to the render this */
    if ( glIsTexture( this->textureID[0] ) ) {
        DWRITE( "texture already exists\n" );
        if ( this->texsize != origtexsize ) {
            DWRITE( "deleting existing texture due to resize\n" );
            glDeleteTextures( 1, &this->textureID[0] );
          }
      }

    if ( !glIsTexture( this->textureID[0] ) ) {
        glGenTextures(1, &this->textureID[0] );

        DWRITE( "allocate texture: %d\ttexsize: %d x %d\n", 
                this->textureID[0], this->texsize, this->texsize );
        glBindTexture(GL_TEXTURE_2D, this->textureID[0] );
        //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        glTexImage2D(GL_TEXTURE_2D,
		    0,
		    GL_RGBA,
		    this->texsize, this->texsize,
		    0,
		    GL_RGBA,
		    GL_UNSIGNED_BYTE,
		    NULL);
      }

    this->useFBO=0;
    return;
  }

  void RenderTarget::fallbackRescale(int width, int height)
  {
	int mindim = width < height ? width : height;
    int origtexsize = this->texsize;
    this->texsize = nearestPower2( mindim, SCALE_MINIFY );      

    /* Create the texture that will be bound to the render this */
    if ( glIsTexture( this->textureID[0] ) ) {
        DWRITE( "texture already exists\n" );
        if ( this->texsize != origtexsize ) {
            DWRITE( "deleting existing texture due to resize\n" );
            glDeleteTextures( 1, &this->textureID[0] );
          }
      }

    if ( !glIsTexture( this->textureID[0] ) ) {
        glGenTextures(1, &this->textureID[0] );

        DWRITE( "allocate texture: %d\ttexsize: %d x %d\n", 
                this->textureID[0], this->texsize, this->texsize );
        glBindTexture(GL_TEXTURE_2D, this->textureID[0] );
        //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        glTexImage2D(GL_TEXTURE_2D,
		    0,
		    GL_RGBA,
		    this->texsize, this->texsize,
		    0,
		    GL_RGBA,
		    GL_UNSIGNED_BYTE,
		    NULL);
      }

  }

/** Destroys the pbuffer */

/** Locks the pbuffer */
void RenderTarget::lock() {


  if(this->useFBO)
    { 
      glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, this->fbuffer[0]);     
    }
  
    }

/** Unlocks the pbuffer */
void RenderTarget::unlock() {


  if(this->useFBO)
    {
      glBindTexture( GL_TEXTURE_2D, this->textureID[1] );
      glCopyTexSubImage2D( GL_TEXTURE_2D,
                         0, 0, 0, 0, 0, 
                         this->texsize, this->texsize );
      glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
      return;
    }

    /** Fallback texture path */
    DWRITE( "copying framebuffer to texture\n" );
    glBindTexture( GL_TEXTURE_2D, this->textureID[0] );
    
	glCopyTexSubImage2D( GL_TEXTURE_2D, 0, 0, 0, 0, 0, this->texsize, this->texsize );
  }

/** 
 * Calculates the nearest power of two to the given number using the
 * appropriate rule
 */
int RenderTarget::nearestPower2( int value, TextureScale scaleRule ) {

    int x = value;
    int power = 0;

    DWRITE( "nearestPower2(): %d\n", value );

    while ( ( x & 0x01 ) != 1 ) {
        x >>= 1;
      }

    if ( x == 1 ) {
        return value;
      } else {
        x = value;
        while ( x != 0 ) {
            x >>= 1;
            power++;
          }
        switch ( scaleRule ) {
            case SCALE_NEAREST:
                if ( ( ( 1 << power ) - value ) <= ( value - ( 1 << ( power - 1 ) ) ) ) {
                    return 1 << power;
                  } else {
                    return 1 << ( power - 1 );
                  }
            case SCALE_MAGNIFY:
                return 1 << power;
            case SCALE_MINIFY:
                return 1 << ( power - 1 );
            default:
                break;
          }
      }
    return 0;
  }

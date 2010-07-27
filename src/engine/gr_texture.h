//========================================================================
//	file:		gr_texture.h
//	author:		Shawn Presser 
//	date:		7/2/10
//
// (c) 2010 Shawn Presser.  All Rights Reserved.
//========================================================================
#pragma once

//========================================================================
// Declarations
//========================================================================
class GL2Driver;
class D3D9Driver;
class Internal_GrDriver;
//========================================================================

//========================================================================
// GrTexture
//========================================================================
class ENGINE_API GrTexture
{
	friend class GL2Driver;
	friend class D3D9Driver;
private:
	Internal_GrDriver*	_driver;
	void*				_userdata;

	// vertex data.
	byte*				_pixels;
	uint				_width;
	uint				_height;

	GrTexture(Internal_GrDriver* driver);
public:
	~GrTexture();

	// provide access to texture data.
	const byte*			GetPixel(uint x, uint y) const	{ E_VERIFY(x < _width && y < _height, return NULL); return _pixels + 4*_width*y + 4*x; }
	uint				GetWidth() const				{ return _width; }
	uint				GetHeight() const				{ return _height; }
	void*				GetUserdata() const				{ return _userdata; }
};
//========================================================================




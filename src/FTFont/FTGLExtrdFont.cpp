/*
 * FTGL - OpenGL font library
 *
 * Copyright (c) 2001-2004 Henry Maddocks <ftgl@opengl.geek.nz>
 *
 * Permission is hereby granted, free of charge, to any person obtaining
 * a copy of this software and associated documentation files (the
 * "Software"), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish,
 * distribute, sublicense, and/or sell copies of the Software, and to
 * permit persons to whom the Software is furnished to do so, subject to
 * the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
 * CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
 * TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
 * SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

#include "config.h"

#include "FTGL/ftgl.h"

#include "FTInternals.h"
#include "FTGLExtrdFontImpl.h"


//
//  FTGLExtrdFont
//


FTGLExtrdFont::FTGLExtrdFont(char const *fontFilePath)
{
    impl = new FTGLExtrdFontImpl(fontFilePath);
}


FTGLExtrdFont::FTGLExtrdFont(const unsigned char *pBufferBytes,
                             size_t bufferSizeInBytes)
{
    impl = new FTGLExtrdFontImpl(pBufferBytes, bufferSizeInBytes);
}


FTGLExtrdFont::~FTGLExtrdFont()
{
    ;
}


//
//  FTGLExtrdFontImpl
//


FTGlyph* FTGLExtrdFontImpl::MakeGlyph(unsigned int glyphIndex)
{
    FT_GlyphSlot ftGlyph = face.Glyph(glyphIndex, FT_LOAD_NO_HINTING);

    if(ftGlyph)
    {
        FTExtrdGlyph* tempGlyph = new FTExtrdGlyph(ftGlyph, depth, front, back, useDisplayLists);
        return tempGlyph;
    }

    err = face.Error();
    return NULL;
}

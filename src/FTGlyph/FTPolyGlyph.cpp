/*
 * FTGL - OpenGL font library
 *
 * Copyright (c) 2001-2004 Henry Maddocks <ftgl@opengl.geek.nz>
 *               2008 Éric Beets <ericbeets@free.fr>
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
#include "FTPolyGlyphImpl.h"
#include "FTVectoriser.h"


//
//  FTGLPolyGlyph
//


FTPolyGlyph::FTPolyGlyph(FT_GlyphSlot glyph, float outset,
                         bool useDisplayList)
{
    impl = new FTPolyGlyphImpl(glyph, outset, useDisplayList);
}


FTPolyGlyph::~FTPolyGlyph()
{
    ;
}


//
//  FTGLPolyGlyphImpl
//


FTPolyGlyphImpl::FTPolyGlyphImpl(FT_GlyphSlot glyph, float _outset,
                                 bool useDisplayList)
:   FTGlyphImpl(glyph),
    glList(0)
{
    if(ft_glyph_format_outline != glyph->format)
    {
        err = 0x14; // Invalid_Outline
        return;
    }

    vectoriser = new FTVectoriser(glyph);

    if((vectoriser->ContourCount() < 1) || (vectoriser->PointCount() < 3))
    {
        delete vectoriser;
        vectoriser = NULL;
        return;
    }


    hscale = glyph->face->size->metrics.x_ppem * 64;
    vscale = glyph->face->size->metrics.y_ppem * 64;
    outset = _outset;

    if(useDisplayList)
    {
        glList = glGenLists(1);
        glNewList(glList, GL_COMPILE);

        DoRender();

        glEndList();

        delete vectoriser;
        vectoriser = NULL;
    }
}


FTPolyGlyphImpl::~FTPolyGlyphImpl()
{
    if(glList)
    {
        glDeleteLists(glList, 1);
    }
    else if(vectoriser)
    {
        delete vectoriser;
    }
}


const FTPoint& FTPolyGlyphImpl::Render(const FTPoint& pen, int renderMode)
{
    glTranslatef(pen.X(), pen.Y(), 0.0f);
    if(glList)
    {
        glCallList(glList);
    }
    else if(vectoriser)
    {
        DoRender();
    }
    glTranslatef(-pen.X(), -pen.Y(), 0.0f);

    return advance;
}


void FTPolyGlyphImpl::DoRender()
{
    vectoriser->MakeMesh(1.0, 1, outset);

    const FTMesh *mesh = vectoriser->GetMesh();

    for(unsigned int t = 0; t < mesh->TesselationCount(); ++t)
    {
        const FTTesselation* subMesh = mesh->Tesselation(t);
        unsigned int polygonType = subMesh->PolygonType();

        glBegin(polygonType);
            for(unsigned int i = 0; i < subMesh->PointCount(); ++i)
            {
                FTPoint point = subMesh->Point(i);
                glTexCoord2f(point.X() / hscale, point.Y() / vscale);
                glVertex3f(point.X() / 64.0f, point.Y() / 64.0f, 0.0f);
            }
        glEnd();
    }
}

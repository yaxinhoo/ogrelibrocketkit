/*
 * This source file is part of libRocket, the HTML/CSS Interface Middleware
 *
 * For the latest information, see http://www.librocket.com
 *
 * Copyright (c) 2008-2010 CodePoint Ltd, Shift Technology Ltd
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 *
 */

#include "RenderInterfaceOgre3D.h"
#include <Ogre.h>
#include "gkGUI.h"
#include "OgreFont.h"
#include "FreeImageDebug.h"

struct RocketOgre3DVertex
{
	float x, y, z;
	Ogre::uint32 diffuse;
	float u, v;
};

// The structure created for each texture loaded by Rocket for Ogre.
struct RocketOgre3DTexture
{
	RocketOgre3DTexture(Ogre::TexturePtr texture) : texture(texture)
	{
	}

	Ogre::TexturePtr texture;
};

// The structure created for each set of geometry that Rocket compiles. It stores the vertex and index buffers and the
// texture associated with the geometry, if one was specified.
struct RocketOgre3DCompiledGeometry
{
	Ogre::RenderOperation render_operation;
	RocketOgre3DTexture* texture;
};

RenderInterfaceOgre3D::RenderInterfaceOgre3D(unsigned int window_width, unsigned int window_height)
{
	render_system = Ogre::Root::getSingleton().getRenderSystem();

	// Configure the colour blending mode.
	colour_blend_mode.blendType = Ogre::LBT_COLOUR;
	colour_blend_mode.source1 = Ogre::LBS_DIFFUSE;
	colour_blend_mode.source2 = Ogre::LBS_TEXTURE;
	colour_blend_mode.operation = Ogre::LBX_MODULATE;

	// Configure the alpha blending mode.
	alpha_blend_mode.blendType = Ogre::LBT_ALPHA;
	alpha_blend_mode.source1 = Ogre::LBS_DIFFUSE;
	alpha_blend_mode.source2 = Ogre::LBS_TEXTURE;
	alpha_blend_mode.operation = Ogre::LBX_MODULATE;

	scissor_enable = false;

	scissor_left = 0;
	scissor_top = 0;
	scissor_right = (int) window_width;
	scissor_bottom = (int) window_height;
    
    isrendergeometry = true;
}

RenderInterfaceOgre3D::~RenderInterfaceOgre3D()
{
}

// Called by Rocket when it wants to render geometry that it does not wish to optimise.
void RenderInterfaceOgre3D::RenderGeometry(Rocket::Core::Vertex* vertices, int num_vertices, int* indices, int num_indices, Rocket::Core::TextureHandle texture, const Rocket::Core::Vector2f& translation)
{
	// We've chosen to not support non-compiled geometry in the Ogre3D renderer.
	// But if you want, you can uncomment this code, so borders will be shown.
	
	Rocket::Core::CompiledGeometryHandle gh = CompileGeometry(vertices, num_vertices, indices, num_indices, texture);
	RenderCompiledGeometry(gh, translation);
	ReleaseCompiledGeometry(gh);
    isrendergeometry = true;
}

// Called by Rocket when it wants to compile geometry it believes will be static for the forseeable future.
Rocket::Core::CompiledGeometryHandle RenderInterfaceOgre3D::CompileGeometry(Rocket::Core::Vertex* vertices, int num_vertices, int* indices, int num_indices, Rocket::Core::TextureHandle texture)
{
	RocketOgre3DCompiledGeometry* geometry = new RocketOgre3DCompiledGeometry();
	geometry->texture = texture == NULL ? NULL : (RocketOgre3DTexture*) texture;

	geometry->render_operation.vertexData = new Ogre::VertexData();
	geometry->render_operation.vertexData->vertexStart = 0;
	geometry->render_operation.vertexData->vertexCount = num_vertices;

	geometry->render_operation.indexData = new Ogre::IndexData();
	geometry->render_operation.indexData->indexStart = 0;
	geometry->render_operation.indexData->indexCount = num_indices;

	geometry->render_operation.operationType = Ogre::RenderOperation::OT_TRIANGLE_LIST;

	// Set up the vertex declaration.
	Ogre::VertexDeclaration* vertex_declaration = geometry->render_operation.vertexData->vertexDeclaration;
	size_t element_offset = 0;
	vertex_declaration->addElement(0, element_offset, Ogre::VET_FLOAT3, Ogre::VES_POSITION);
	element_offset += Ogre::VertexElement::getTypeSize(Ogre::VET_FLOAT3);
	vertex_declaration->addElement(0, element_offset, Ogre::VET_COLOUR, Ogre::VES_DIFFUSE);
	element_offset += Ogre::VertexElement::getTypeSize(Ogre::VET_COLOUR);
	vertex_declaration->addElement(0, element_offset, Ogre::VET_FLOAT2, Ogre::VES_TEXTURE_COORDINATES);

#if GK_PLATFORM == GK_PLATFORM_APPLE_IOS
	// Create the vertex buffer.
	Ogre::HardwareVertexBufferSharedPtr vertex_buffer = Ogre::HardwareBufferManager::getSingleton().createVertexBuffer(vertex_declaration->getVertexSize(0), num_vertices, Ogre::HardwareBuffer::HBU_STATIC_WRITE_ONLY);
    geometry->render_operation.vertexData->vertexBufferBinding->setBinding(0, vertex_buffer);
    
	// Fill the vertex buffer.
	RocketOgre3DVertex* ogre_vertices = (RocketOgre3DVertex*) vertex_buffer->lock(0, vertex_buffer->getSizeInBytes(), Ogre::HardwareBuffer::HBL_NORMAL);
#else
    // Create the vertex buffer.
    Ogre::HardwareVertexBufferSharedPtr vertex_buffer = Ogre::HardwareBufferManager::getSingleton().createVertexBuffer(vertex_declaration->getVertexSize(0), num_vertices, Ogre::HardwareBuffer::HBU_STATIC_WRITE_ONLY);
    geometry->render_operation.vertexData->vertexBufferBinding->setBinding(0, vertex_buffer);
    
	// Fill the vertex buffer.
	RocketOgre3DVertex* ogre_vertices = (RocketOgre3DVertex*) vertex_buffer->lock(0, vertex_buffer->getSizeInBytes(), Ogre::HardwareBuffer::HBL_NORMAL);
#endif
	
	for (int i = 0; i < num_vertices; ++i)
	{
		ogre_vertices[i].x = vertices[i].position.x;
		ogre_vertices[i].y = vertices[i].position.y;
		ogre_vertices[i].z = 0;

		Ogre::ColourValue diffuse(vertices[i].colour.red / 255.0f, vertices[i].colour.green / 255.0f, vertices[i].colour.blue / 255.0f, vertices[i].colour.alpha / 255.0f);
		render_system->convertColourValue(diffuse, &ogre_vertices[i].diffuse);

		ogre_vertices[i].u = vertices[i].tex_coord[0];
		ogre_vertices[i].v = vertices[i].tex_coord[1];
	}
	vertex_buffer->unlock();

#if GK_PLATFORM == GK_PLATFORM_APPLE_IOS
	// Create the index buffer.
	Ogre::HardwareIndexBufferSharedPtr index_buffer = Ogre::HardwareBufferManager::getSingleton().createIndexBuffer(Ogre::HardwareIndexBuffer::IT_16BIT, num_indices, Ogre::HardwareBuffer::HBU_STATIC_WRITE_ONLY);
    geometry->render_operation.indexData->indexBuffer = index_buffer;
	geometry->render_operation.useIndexes = true;
#else
    // Create the index buffer.
	Ogre::HardwareIndexBufferSharedPtr index_buffer = Ogre::HardwareBufferManager::getSingleton().createIndexBuffer(Ogre::HardwareIndexBuffer::IT_32BIT, num_indices, Ogre::HardwareBuffer::HBU_STATIC_WRITE_ONLY);    
    geometry->render_operation.indexData->indexBuffer = index_buffer;
	geometry->render_operation.useIndexes = true;
#endif
	
	// Fill the index buffer.
#if GK_PLATFORM == GK_PLATFORM_APPLE_IOS    
    unsigned short * ogre_indices = (unsigned short*)index_buffer->lock(0, index_buffer->getSizeInBytes(), Ogre::HardwareBuffer::HBL_NORMAL);
    for(int i=0;i<num_indices;i++)
        ogre_indices[i] = indices[i];
#else
    unsigned int * ogre_indices = (unsigned int*)index_buffer->lock(0, index_buffer->getSizeInBytes(), Ogre::HardwareBuffer::HBL_NORMAL);
	memcpy(ogre_indices, indices, sizeof(unsigned int) * num_indices);
#endif
	index_buffer->unlock();
		
    isrendergeometry = false;
	return reinterpret_cast<Rocket::Core::CompiledGeometryHandle>(geometry);
    //return 0;
}

// Called by Rocket when it wants to render application-compiled geometry.
void RenderInterfaceOgre3D::RenderCompiledGeometry(Rocket::Core::CompiledGeometryHandle geometry, const Rocket::Core::Vector2f& translation)
{
    if(isrendergeometry)
    {
        RenderInterface::RenderCompiledGeometry(geometry, translation);
    }else
    {
        Ogre::Matrix4 transform;
        transform.makeTrans(translation.x, translation.y, 0);
        render_system->_setWorldMatrix(transform);

        render_system = Ogre::Root::getSingleton().getRenderSystem();
        RocketOgre3DCompiledGeometry* ogre3d_geometry = (RocketOgre3DCompiledGeometry*) geometry;
        if (ogre3d_geometry->texture != NULL)
        {
            int count=render_system->getCapabilities()->getNumTextureUnits();
            render_system->_setTexture(0, true, ogre3d_geometry->texture->texture);

            // Ogre can change the blending modes when textures are disabled - so in case the last render had no texture,
            // we need to re-specify them.
            render_system->_setTextureBlendMode(0, colour_blend_mode);
            render_system->_setTextureBlendMode(0, alpha_blend_mode);
        }
        else
            render_system->_disableTextureUnit(0);

        render_system->_render(ogre3d_geometry->render_operation);
    }
}

// Called by Rocket when it wants to release application-compiled geometry.
void RenderInterfaceOgre3D::ReleaseCompiledGeometry(Rocket::Core::CompiledGeometryHandle geometry)
{
	RocketOgre3DCompiledGeometry* ogre3d_geometry = reinterpret_cast<RocketOgre3DCompiledGeometry*>(geometry);
	delete ogre3d_geometry->render_operation.vertexData;
	delete ogre3d_geometry->render_operation.indexData;
	delete ogre3d_geometry;
}

// Called by Rocket when it wants to enable or disable scissoring to clip content.
void RenderInterfaceOgre3D::EnableScissorRegion(bool enable)
{
	scissor_enable = enable;

	if (!scissor_enable)
		render_system->setScissorTest(false);
	else
		render_system->setScissorTest(true, scissor_left, scissor_top, scissor_right, scissor_bottom);
}

// Called by Rocket when it wants to change the scissor region.
void RenderInterfaceOgre3D::SetScissorRegion(int x, int y, int width, int height)
{
	scissor_left = x;
	scissor_top = y;
	scissor_right = x + width;
	scissor_bottom = y + height;

	if (scissor_enable)
		render_system->setScissorTest(true, scissor_left, scissor_top, scissor_right, scissor_bottom);
}

// Called by Rocket when a texture is required by the library.
bool RenderInterfaceOgre3D::LoadTexture(Rocket::Core::TextureHandle& texture_handle, Rocket::Core::Vector2i& texture_dimensions, const Rocket::Core::String& source)
{
	Ogre::TextureManager* texture_manager = Ogre::TextureManager::getSingletonPtr();
	Ogre::TexturePtr ogre_texture = texture_manager->getByName(Ogre::String(source.CString()));
	if (ogre_texture.isNull())
	{
		ogre_texture = texture_manager->load(Ogre::String(source.CString()),
							DEFAULT_ROCKET_RESOURCE_GROUP,
							Ogre::TEX_TYPE_2D,
							0);
	}

	if (ogre_texture.isNull())
		return false;

	texture_dimensions.x = ogre_texture->getWidth();
	texture_dimensions.y = ogre_texture->getHeight();

	texture_handle = reinterpret_cast<Rocket::Core::TextureHandle>(new RocketOgre3DTexture(ogre_texture));
	return true;
}


bool saveTextureToLocal(Ogre::TexturePtr texPtr, const Ogre::String& fileName)
{
    Ogre::HardwarePixelBufferSharedPtr tmpTexBuf = texPtr->getBuffer();
    int width = texPtr->getWidth();
    int height= texPtr->getHeight();
    char* tmpBuf = new char[width*height*4];
    Ogre::PixelBox tmpBox(width,
                          height,
                          texPtr->getDepth(),
                          texPtr->getFormat(),
                          tmpBuf);
    tmpTexBuf->blitToMemory(tmpBox);
    Ogre::TexturePtr tmpTexPtr = Ogre::TextureManager::getSingleton().createManual ( fileName, "General",
                                                                                    Ogre::TEX_TYPE_2D, texPtr->getWidth(), texPtr->getHeight(), 1, 0, Ogre::PF_A8B8G8R8 );
    tmpTexPtr->load();
    
    /*
    HRESULT hr;
    Ogre::IDirect3DSurface9 *pDstSurface = 0;
    Ogre::D3D9Texture *d3dTex = reinterpret_cast<  Ogre::D3D9Texture * >(tmpTexPtr.get());
    RECT dstRC = {0, 0, d3dTex->getWidth(), d3dTex->getHeight()};
    if( FAILED( hr = d3dTex->getNormTexture()->GetSurfaceLevel(0, &pDstSurface) ) )
    {
        return false;
    }
    */
    
    size_t rowWidth;
    if ( Ogre::PixelUtil::isCompressed(tmpBox.format))
    {
        // D3D wants the width of one row of cells in bytes
        if (tmpBox.format == Ogre::PF_DXT1)
        {
            // 64 bits (8 bytes) per 4x4 block
            rowWidth = (tmpBox.rowPitch / 4) * 8;
        }
        else
        {
            // 128 bits (16 bytes) per 4x4 block
            rowWidth = (tmpBox.rowPitch / 4) * 16;
        }
    }
    else
    {
        rowWidth = tmpBox.rowPitch * Ogre::PixelUtil::getNumElemBytes(tmpBox.format);
    }
    
    /*
    if( FAILED( hr = D3DXLoadSurfaceFromMemory(
                                               pDstSurface, NULL, &dstRC, tmpBox.data,
                                               Ogre::D3D9Mappings::_getPF( tmpBox.format),
                                               rowWidth, 0,
                                               &dstRC, D3DX_FILTER_TRIANGLE | D3DX_FILTER_DITHER, 0)) )
    {
        SAFE_RELEASE(pDstSurface);
        return false;
    }
    */
    
    Ogre::HardwarePixelBufferSharedPtr tmpDesTexBuf = tmpTexPtr->getBuffer();
    
    char* tmpDesBuf = new char[width*height*4];
    
    Ogre::PixelBox tmpDesBox(width,
                             height,
                             tmpTexPtr->getDepth(),
                             tmpTexPtr->getFormat(),
                             tmpDesBuf);
    
    tmpDesTexBuf->blitToMemory(tmpDesBox);
    Ogre::Image image;
    image.loadDynamicImage((Ogre::uchar*)tmpDesBox.data, tmpDesBox.getWidth(), tmpDesBox.getHeight(), tmpDesBox.format);
    image.save(fileName);
    delete [] tmpDesBuf; 
    delete [] tmpBuf;
    
    tmpTexPtr.setNull();
    return true;
}


Ogre::TexturePtr getFontTexture(Ogre::FontPtr fp)
{
    // Make sure the font is loaded
    fp->load();
    
    return
    static_cast<Ogre::TexturePtr>
    (
     Ogre::TextureManager::getSingleton().getByName(fp->getMaterial()->getTechnique(0)->getPass(0)->getTextureUnitState(0)->getTextureName())
     );
}

void saveTextureToFile(Ogre::TexturePtr tp,Ogre::String fileName)
{
    // Declare buffer
    const size_t buffSize = tp->getWidth()*tp->getHeight()*4;
    unsigned char *data = OGRE_ALLOC_T(unsigned char,buffSize,Ogre::MEMCATEGORY_GENERAL);
    
    // Clear buffer
    memset(data, 0, buffSize);
    
    // Setup Image with correct settings
    Ogre::Image i;
    i.loadDynamicImage(data, tp->getWidth(), tp->getHeight(), 1, Ogre::PF_R8G8B8A8, true);
    
    // Copy Texture buffer contents to image buffer
    Ogre::HardwarePixelBufferSharedPtr buf = tp->getBuffer();
    const Ogre::PixelBox destBox = i.getPixelBox();
    buf->blitToMemory(destBox);
    
    // Save to disk!
    i.save(fileName);
}

void saveFontTextureToFile(Ogre::FontPtr fp, const Ogre::String& fileName)
{
    Ogre::TexturePtr tp = getFontTexture(fp);
    saveTextureToFile(tp,fileName);
}

// Called by Rocket when a texture is required to be built from an internally-generated sequence of pixels.
bool RenderInterfaceOgre3D::GenerateTexture(Rocket::Core::TextureHandle& texture_handle, const Rocket::Core::byte* source, const Rocket::Core::Vector2i& source_dimensions)
{
	static int texture_id = 1;

    Ogre::DataStreamPtr stream(OGRE_NEW Ogre::MemoryDataStream((void*) source, source_dimensions.x * source_dimensions.y * sizeof(unsigned int)));
    Rocket::Core::String s = Rocket::Core::String(16, "%d", texture_id++);
    //Rocket::Core::Log::Message(Rocket::Core::Log::LT_INFO,"loadRawData %s",s.CString());
	Ogre::TexturePtr ogre_texture = Ogre::TextureManager::getSingleton().loadRawData(
										s.CString(),
										DEFAULT_ROCKET_RESOURCE_GROUP,
										stream,
										(Ogre::ushort)source_dimensions.x,
										(Ogre::ushort)source_dimensions.y,
										Ogre::PF_A8B8G8R8,
                                        //Ogre::PF_A8R8G8B8,
										Ogre::TEX_TYPE_2D,
										0);

	if (ogre_texture.isNull())
		return false;
    
    char file[512];
    sprintf(file, "/Volumes/MacData/Work/cbuild/ogrekites1/Bin/Debug-iphonesimulator/fontimage/%s_RIO3D.jpg",s.CString());
    //ByteToFile((void*)source,source_dimensions.x,source_dimensions.y, std::string(file));
    //saveTextureToFile(ogre_texture,Ogre::String(file));
	texture_handle = reinterpret_cast<Rocket::Core::TextureHandle>(new RocketOgre3DTexture(ogre_texture));
	return true;
}

// Called by Rocket when a loaded texture is no longer required.
void RenderInterfaceOgre3D::ReleaseTexture(Rocket::Core::TextureHandle texture)
{
	delete ((RocketOgre3DTexture*) texture);
}

// Returns the native horizontal texel offset for the renderer.
float RenderInterfaceOgre3D::GetHorizontalTexelOffset()
{
	return -render_system->getHorizontalTexelOffset();
}

// Returns the native vertical texel offset for the renderer.
float RenderInterfaceOgre3D::GetVerticalTexelOffset()
{
	return -render_system->getVerticalTexelOffset();
}

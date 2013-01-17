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
/*  RocketApplication.cpp is modifed for OgreKit by harkon.kr.
	http://gamekit.googlecode.com/
*/

#include "RocketRenderListener.h"
#include <Rocket/Core.h>
#include <Rocket/Controls.h>
#include <Rocket/Debugger.h>
#include "RenderInterfaceOgre3D.h"
#include "RocketEventListener.h"
#include "SystemInterfaceOgre3D.h"
#include "FileInterfaceOgre3D.h"


using namespace Ogre;


RocketRenderListener::RocketRenderListener(Ogre::RenderWindow *window,Ogre::SceneManager* mgr, Rocket::Core::Context* context)
	:	m_context(context),
		m_window(window),
        m_sceneMgr(mgr)
{
	// Add the application as a listener to Ogre's render queue so we can render during the overlay.
	m_window->addListener(this);
}

RocketRenderListener::~RocketRenderListener()
{
	m_window->removeListener(this);
}


void RocketRenderListener::postRenderTargetUpdate(const Ogre::RenderTargetEvent &evt)
{
	if (Ogre::Root::getSingleton().getRenderSystem()->_getViewport()->getOverlaysEnabled())
	{
		m_context->Update();

		ConfigureRenderSystem();
		m_context->Render();
	}
}

// Configures Ogre's rendering system for rendering Rocket.
void RocketRenderListener::ConfigureRenderSystem()
{
	Ogre::RenderSystem* render_system = Ogre::Root::getSingleton().getRenderSystem();
    
    //m_sceneMgr = Ogre::Root::getSingleton()._getCurrentSceneManager();
    
	// Set up the projection and view matrices.
	Ogre::Matrix4 projection_matrix;
	BuildProjectionMatrix(projection_matrix);
	render_system->_setProjectionMatrix(projection_matrix);
	//render_system->_setWorldMatrix(Ogre::Matrix4::IDENTITY);
    render_system->_setViewMatrix(Ogre::Matrix4::IDENTITY);

	// Disable lighting, as all of Rocket's geometry is unlit.
	render_system->setLightingEnabled(false);
	// Disable depth-buffering; all of the geometry is already depth-sorted.
	render_system->_setDepthBufferParams(false, false);
    // Disable depth bias.
	render_system->_setDepthBias(0, 0);
    
	// Rocket generates anti-clockwise geometry, so enable clockwise-culling.
	render_system->_setCullingMode(Ogre::CULL_CLOCKWISE);
	// Disable fogging.
	render_system->_setFog(Ogre::FOG_NONE);
	// Enable writing to all four channels.
	render_system->_setColourBufferWriteEnabled(true, true, true, true);
	// Unbind any vertex or fragment programs bound previously by the application.
    
#if GK_PLATFORM != GK_PLATFORM_APPLE_IOS    
	render_system->unbindGpuProgram(Ogre::GPT_FRAGMENT_PROGRAM);
	render_system->unbindGpuProgram(Ogre::GPT_VERTEX_PROGRAM);
#else
#if !defined(OGRE_BUILD_RENDERSYSTEM_GLES2)
        render_system->unbindGpuProgram(Ogre::GPT_FRAGMENT_PROGRAM);
        render_system->unbindGpuProgram(Ogre::GPT_VERTEX_PROGRAM);
#endif
    //Ogre::MaterialPtr material = Ogre::MaterialManager::getSingleton().getDefaultSettings();
    //Ogre::MaterialPtr material = Ogre::MaterialManager::getSingleton();
    //Ogre::Pass* pass=material->getTechnique(0)->getPass(0);
    //m_sceneMgr->updateGpuProgramParameters(pass);
    //render_system->bindGpuProgram(pass->getVertexProgram()->_getBindingDelegate());
#endif
    render_system->setShadingType(Ogre::SO_GOURAUD);

	// Set texture settings to clamp along both axes.
	Ogre::TextureUnitState::UVWAddressingMode addressing_mode;
	addressing_mode.u = Ogre::TextureUnitState::TAM_CLAMP;
	addressing_mode.v = Ogre::TextureUnitState::TAM_CLAMP;
	addressing_mode.w = Ogre::TextureUnitState::TAM_CLAMP;
	render_system->_setTextureAddressingMode(0, addressing_mode);

	// Set the texture coordinates for unit 0 to be read from unit 0.
	render_system->_setTextureCoordSet(0, 0);
	// Disable texture coordinate calculation.
	render_system->_setTextureCoordCalculation(0, Ogre::TEXCALC_NONE);
	// Enable linear filtering; images should be rendering 1 texel == 1 pixel, so point filtering could be used
	// except in the case of scaling tiled decorators.
	render_system->_setTextureUnitFiltering(0, Ogre::FO_LINEAR, Ogre::FO_LINEAR, Ogre::FO_POINT);
	// Disable texture coordinate transforms.
	render_system->_setTextureMatrix(0, Ogre::Matrix4::IDENTITY);
	// Reject pixels with an alpha of 0.
	render_system->_setAlphaRejectSettings(Ogre::CMPF_GREATER, 0, false);
	// Disable all texture units but the first.
    
    Ogre::LayerBlendModeEx mColorBlendMode, mAlphaBlendMode;
    mColorBlendMode.blendType = Ogre::LBT_COLOUR;
    mColorBlendMode.source1 = Ogre::LBS_TEXTURE;
    mColorBlendMode.source2 = Ogre::LBS_DIFFUSE;
    mColorBlendMode.operation = Ogre::LBX_MODULATE;
    
    mAlphaBlendMode.blendType = Ogre::LBT_ALPHA;
    mAlphaBlendMode.source1 = Ogre::LBS_TEXTURE;
    mAlphaBlendMode.source2 = Ogre::LBS_DIFFUSE;
    mAlphaBlendMode.operation = Ogre::LBX_MODULATE;
    
	render_system->_setTextureBlendMode(0, mColorBlendMode);
    render_system->_setTextureBlendMode(0, mAlphaBlendMode);
    render_system->_disableTextureUnitsFrom(1);

	// Enable simple alpha blending.
	render_system->_setSceneBlending(Ogre::SBF_SOURCE_ALPHA, Ogre::SBF_ONE_MINUS_SOURCE_ALPHA);

	render_system->_setPolygonMode(Ogre::PM_SOLID);
}

// Builds an OpenGL-style orthographic projection matrix.
void RocketRenderListener::BuildProjectionMatrix(Ogre::Matrix4& projection_matrix)
{
	float z_near = -1;
	float z_far = 1;

	projection_matrix = Ogre::Matrix4::ZERO;

	// Set up matrices.
	projection_matrix[0][0] = 2.0f / m_window->getWidth();
	projection_matrix[0][3]= -1.0000000f;
	projection_matrix[1][1]= -2.0f / m_window->getHeight();
	projection_matrix[1][3]= 1.0000000f;
	projection_matrix[2][2]= -2.0f / (z_far - z_near);
	projection_matrix[3][3]= 1.0000000f;
}

#pragma once


#include "../Dependencies/Eos/Eos/Eos.h"
//#include "../Dependencies/Nix/Nix/Nix.h"

#include "../Core/CoreDefs.h"

#include "../Texture/Texture.h"
//#include "RenderDefs.h"


#define ION_MAX_RENDER_COMMAND  16


EOS_USING_NAMESPACE
//NIX_USING_NAMESPACE


ION_NAMESPACE_BEGIN

enum ERenderOperation 
{
    ERenderOperation_None = 0,
    ERenderOperation_Draw_View,
    ERenderOperation_Copy_Render,
};

struct RenderCommand final
{
    ERenderOperation	m_operation;
    Texture*	        m_texture;
    //viewDef_t *	    m_viewDef;      // does not exists at the moment
    ionS32			    m_x;
    ionS32			    m_y;
    ionS32			    m_textureWidth;
    ionS32			    m_textureHeight;
    ionS32			    m_cubeFace; // when copying to a cubeMap
    ionBool		        m_clearColorAfterCopy;

    RenderCommand() :
        m_operation(ERenderOperation_None),
        m_texture(nullptr),
        //viewDef(NULL),
        m_x(0),
        m_y(0),
        m_textureWidth(0),
        m_textureHeight(0),
        m_cubeFace(0),
        m_clearColorAfterCopy(false) {}
};


class FrameData final
{
private:
    typedef std::atomic_size_t   AtomicSize;

public:
    FrameData() :
        m_frameMemory(nullptr),
        m_renderCommandIndex(0)
    {
        m_frameMemoryAllocated.fetch_add(0, std::memory_order_relaxed);
        m_frameMemoryUsed.fetch_add(0, std::memory_order_relaxed);
        m_renderCommands.resize(ION_MAX_RENDER_COMMAND, RenderCommand());
    }

    AtomicSize	                m_frameMemoryAllocated;
    AtomicSize	                m_frameMemoryUsed;
    ionU8*					    m_frameMemory;
    ionS32						m_renderCommandIndex;
    eosVector(RenderCommand)    m_renderCommands;       // 16 max
};

ION_NAMESPACE_END
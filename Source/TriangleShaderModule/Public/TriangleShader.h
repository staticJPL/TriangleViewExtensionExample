#pragma once
#include "GlobalShader.h"
#include "ShaderParameterMacros.h"
#include "ShaderParameters.h"
#include "RenderResource.h"
#include "RHI.h"
#include "ShaderParameterStruct.h"
#include "Containers/DynamicRHIResourceArray.h"

/** The vertex data used to filter a texture. */
struct FColorVertex
{
public:
	FVector2f Position;
	FVector4f Color;
};

/**
 * Static vertex and index buffer used for 2D screen rectangles.
 */
class FTriangleVertexBuffer : public FVertexBuffer
{
public:
	/** Initialize the RHI for this rendering resource */
	void InitRHI(FRHICommandListBase& RHICmcList) override {

		// This is a Numa Array type, technically I am just using this object to obtain the correct size the RHI CMD will require.
		TResourceArray<FColorVertex, VERTEXBUFFER_ALIGNMENT> Vertices;
		Vertices.SetNumUninitialized(3);
    	
		Vertices[0].Position = FVector2f(0.0f,0.75f);
		Vertices[0].Color = FVector4f(1, 0, 0, 1);
    
		Vertices[1].Position = FVector2f(0.75,-0.75);
		Vertices[1].Color = FVector4f(0, 1, 0, 1);
    
		Vertices[2].Position = FVector2f(-0.75,-0.75);
		Vertices[2].Color = FVector4f(0, 0, 1, 1);
    
		FRHIResourceCreateInfo CreateInfo(TEXT("FScreenRectangleVertexBuffer"), &Vertices);
		// Create Vertex Buffer
		// Since this is Statically allocated once to the GPU, we don't need to lock it on the Render Thread to wait for it to be dealt with.
		VertexBufferRHI = RHICmcList.CreateVertexBuffer(Vertices.GetResourceDataSize(),EBufferUsageFlags::Static,CreateInfo);
		
	}
};
extern TRIANGLESHADERMODULE_API TGlobalResource<FTriangleVertexBuffer> GTriangleVertexBuffer;
class FTriangleIndexBuffer : public FIndexBuffer
{
public:
	/** Initialize the RHI for this rendering resource */
	void InitRHI(FRHICommandListBase& RHICmdList) override
	{
		const uint32 Indices[] = { 0, 1, 2 };

		TResourceArray<uint32, INDEXBUFFER_ALIGNMENT> IndexBuffer;
		uint32 NumIndices = UE_ARRAY_COUNT(Indices);
		IndexBuffer.AddUninitialized(NumIndices);
		FMemory::Memcpy(IndexBuffer.GetData(), Indices, NumIndices * sizeof(uint32));
		// RHICreateIndexBuffer
		FRHIResourceCreateInfo CreateInfo(TEXT("FTriangleIndexBuffer"), &IndexBuffer);
		IndexBufferRHI = RHICmdList.CreateIndexBuffer(sizeof(uint32), IndexBuffer.GetResourceDataSize(),BUF_Static,CreateInfo);
		
	}
};
extern TRIANGLESHADERMODULE_API TGlobalResource<FTriangleIndexBuffer> GTriangleIndexBuffer;

/*Define a Render Resource for our buffer, since the draw primitive uses it*/
/** The filter vertex declaration resource type. */
class FTriangleVertexDeclaration : public FRenderResource
{
public:
	FVertexDeclarationRHIRef VertexDeclarationRHI;

	/** Destructor. */
	virtual ~FTriangleVertexDeclaration() {}

	virtual void InitRHI(FRHICommandListBase& RHICmdList)
	{
		FVertexDeclarationElementList Elements;
		uint32 Stride = sizeof(FColorVertex);
		Elements.Add(FVertexElement(0, STRUCT_OFFSET(FColorVertex, Position), VET_Float2, 0, Stride));
		Elements.Add(FVertexElement(0, STRUCT_OFFSET(FColorVertex, Color), VET_Float4, 1, Stride));
		VertexDeclarationRHI = PipelineStateCache::GetOrCreateVertexDeclaration(Elements);
	}

	virtual void ReleaseRHI()
	{
		VertexDeclarationRHI.SafeRelease();
	}
};
extern TRIANGLESHADERMODULE_API TGlobalResource<FTriangleVertexDeclaration> GTriangleVertexDeclaration;

// Defined here so we can access it in View Extension.
BEGIN_SHADER_PARAMETER_STRUCT(FTriangleVSParams,)
	//SHADER_PARAMETER_STRUCT_ARRAY(FTriangleVertParams,Verticies)
	//RENDER_TARGET_BINDING_SLOTS()
END_SHADER_PARAMETER_STRUCT()
class FTriangleVS : public FGlobalShader
{
public:
	DECLARE_GLOBAL_SHADER(FTriangleVS);
	SHADER_USE_PARAMETER_STRUCT(FTriangleVS, FGlobalShader)
	using FParameters = FTriangleVSParams;
	
	static bool ShouldCompilePermutation(const FGlobalShaderPermutationParameters& Parameters) {
		return true;
	}	
};

BEGIN_SHADER_PARAMETER_STRUCT(FTrianglePSParams,)
	RENDER_TARGET_BINDING_SLOTS()
END_SHADER_PARAMETER_STRUCT()

class FTrianglePS: public FGlobalShader
{
	DECLARE_GLOBAL_SHADER(FTrianglePS);
	using FParameters = FTrianglePSParams;
	SHADER_USE_PARAMETER_STRUCT(FTrianglePS, FGlobalShader)
};



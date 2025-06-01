#pragma once
#include "TriangleShader.h"
#include "SceneViewExtension.h"
#include "RenderResource.h"

class TRIANGLESHADERMODULE_API FTriangleViewExtension : public FSceneViewExtensionBase
{
public:
	
	FTriangleViewExtension(const FAutoRegister& AutoRegister);
	//~ Begin FSceneViewExtensionBase Interface
	virtual void SetupViewFamily(FSceneViewFamily& InViewFamily) override {}
	virtual void SetupView(FSceneViewFamily& InViewFamily, FSceneView& InView) override {};
	virtual void BeginRenderViewFamily(FSceneViewFamily& InViewFamily) override{};
	virtual void PreRenderViewFamily_RenderThread(FRDGBuilder& GraphBuilder, FSceneViewFamily& InViewFamily) override{};
	virtual void PreRenderView_RenderThread(FRDGBuilder& GraphBuilder, FSceneView& InView) override;
	virtual void PostRenderBasePass_RenderThread(FRHICommandListImmediate& RHICmdList, FSceneView& InView) override {};
	virtual void PrePostProcessPass_RenderThread(FRDGBuilder& GraphBuilder, const FSceneView& View, const FPostProcessingInputs& Inputs) override;
	virtual void SubscribeToPostProcessingPass(EPostProcessingPass Pass, const FSceneView& InView, FAfterPassCallbackDelegateArray& InOutPassCallbacks, bool bIsPassEnabled) override;
	//~ End FSceneViewExtensionBase Interface

protected:

	// Copied from PixelShaderUtils
	template <typename TShaderClass>
	static void AddFullscreenPass(
		FRDGBuilder& GraphBuilder,
		const FGlobalShaderMap* GlobalShaderMap,
		FRDGEventName&& PassName,
		const TShaderRef<TShaderClass>& PixelShader,
		typename TShaderClass::FParameters* Parameters,
		const FIntRect& Viewport,
		FRHIBlendState* BlendState = nullptr,
		FRHIRasterizerState* RasterizerState = nullptr,
		FRHIDepthStencilState* DepthStencilState = nullptr,
		uint32 StencilRef = 0);
	
	template <typename TShaderClass>
	static void DrawFullscreenPixelShader(
		FRHICommandList& RHICmdList, 
		const FGlobalShaderMap* GlobalShaderMap,
		const TShaderRef<TShaderClass>& PixelShader,
		const typename TShaderClass::FParameters& Parameters,
		const FIntRect& Viewport,
		FRHIBlendState* BlendState = nullptr,
		FRHIRasterizerState* RasterizerState = nullptr,
		FRHIDepthStencilState* DepthStencilState = nullptr,
		uint32 StencilRef = 0);

	static inline void DrawFullScreenTriangle(FRHICommandList& RHICmdList, uint32 InstanceCount){
		RHICmdList.SetStreamSource(0, GTriangleVertexBuffer.VertexBufferRHI, 0);
		RHICmdList.DrawIndexedPrimitive(
			GTriangleIndexBuffer.IndexBufferRHI,
			/*BaseVertexIndex=*/ 0,
			/*MinIndex=*/ 0,
			/*NumVertices=*/ 3,
			/*StartIndex=*/ 0,
			/*NumPrimitives=*/ 1,
			/*NumInstances=*/ InstanceCount);
	}

	// A delegate that is called when the Tone mapper pass finishes
	FScreenPassTexture TrianglePass_RenderThread(FRDGBuilder& GraphBuilder, const FSceneView& View, const FPostProcessMaterialInputs& Inputs);

	// For now we try to build a vertex buffer and see if it puts some shit in it

public:
	static void RenderTriangle
(
FRDGBuilder& GraphBuilder,
const FGlobalShaderMap* ViewShaderMap,
const FIntRect& View,
const FScreenPassTexture& InSceneColor);
	
};


#include "TriangleViewExtension.h"
#include "TriangleShader.h"
#include "PixelShaderUtils.h"
#include "PostProcess/PostProcessing.h"
#include "PostProcess/PostProcessMaterial.h"
#include "SceneTextureParameters.h"
#include "ShaderParameterStruct.h"

DECLARE_GPU_DRAWCALL_STAT(TrianglePass);

FScreenPassTextureViewportParameters GetTextureViewportParameters(const FScreenPassTextureViewport& InViewport) {
	const FVector2f Extent(InViewport.Extent);
	const FVector2f ViewportMin(InViewport.Rect.Min.X, InViewport.Rect.Min.Y);
	const FVector2f ViewportMax(InViewport.Rect.Max.X, InViewport.Rect.Max.Y);
	const FVector2f ViewportSize = ViewportMax - ViewportMin;

	FScreenPassTextureViewportParameters Parameters;

	if (!InViewport.IsEmpty()) {
		Parameters.Extent = FVector2f(Extent);
		Parameters.ExtentInverse = FVector2f(1.0f / Extent.X, 1.0f / Extent.Y);

		Parameters.ScreenPosToViewportScale = FVector2f(0.5f, -0.5f) * ViewportSize;
		Parameters.ScreenPosToViewportBias = (0.5f * ViewportSize) + ViewportMin;

		Parameters.ViewportMin = InViewport.Rect.Min;
		Parameters.ViewportMax = InViewport.Rect.Max;

		Parameters.ViewportSize = ViewportSize;
		Parameters.ViewportSizeInverse = FVector2f(1.0f / Parameters.ViewportSize.X, 1.0f / Parameters.ViewportSize.Y);

		Parameters.UVViewportMin = ViewportMin * Parameters.ExtentInverse;
		Parameters.UVViewportMax = ViewportMax * Parameters.ExtentInverse;

		Parameters.UVViewportSize = Parameters.UVViewportMax - Parameters.UVViewportMin;
		Parameters.UVViewportSizeInverse = FVector2f(1.0f / Parameters.UVViewportSize.X, 1.0f / Parameters.UVViewportSize.Y);

		Parameters.UVViewportBilinearMin = Parameters.UVViewportMin + 0.5f * Parameters.ExtentInverse;
		Parameters.UVViewportBilinearMax = Parameters.UVViewportMax - 0.5f * Parameters.ExtentInverse;
	}

	return Parameters;
}

FSceneTextureParameters GetSceneTextureParameters(FRDGBuilder& GraphBuilder, const FSceneTextures& SceneTextures)
{
	const auto& SystemTextures = FRDGSystemTextures::Get(GraphBuilder);

	FSceneTextureParameters Parameters;

	// Should always have a depth buffer around allocated, since early z-pass is first.
	Parameters.SceneDepthTexture = SceneTextures.Depth.Resolve;
	Parameters.SceneStencilTexture = GraphBuilder.CreateSRV(FRDGTextureSRVDesc::CreateWithPixelFormat(Parameters.SceneDepthTexture, PF_X24_G8));

	// Registers all the scene texture from the scene context. No fallback is provided to catch mistake at shader parameter validation time
	// when a pass is trying to access a resource before any other pass actually created it.
	Parameters.GBufferVelocityTexture = GetIfProduced(SceneTextures.Velocity);
	Parameters.GBufferATexture = GetIfProduced(SceneTextures.GBufferA);
	Parameters.GBufferBTexture = GetIfProduced(SceneTextures.GBufferB);
	Parameters.GBufferCTexture = GetIfProduced(SceneTextures.GBufferC);
	Parameters.GBufferDTexture = GetIfProduced(SceneTextures.GBufferD);
	Parameters.GBufferETexture = GetIfProduced(SceneTextures.GBufferE);
	Parameters.GBufferFTexture = GetIfProduced(SceneTextures.GBufferF, SystemTextures.MidGrey);

	return Parameters;
}

FVector2f GetInputViewportSize2f(const FIntRect& Input, const FIntPoint& Extent)
{
	// Based on
	// GetScreenPassTextureViewportParameters()
	// Engine/Source/Runtime/Renderer/Private/ScreenPass.cpp

	FVector2f ExtentInverse = FVector2f(1.0f / Extent.X, 1.0f / Extent.Y);

	FVector2f RectMin = FVector2f(Input.Min);
	FVector2f RectMax = FVector2f(Input.Max);

	FVector2f Min = RectMin * ExtentInverse;
	FVector2f Max = RectMax * ExtentInverse;

	return (Max - Min);
}

FTriangleViewExtension::FTriangleViewExtension(const FAutoRegister& AutoRegister) : FSceneViewExtensionBase(AutoRegister) {
}

void FTriangleViewExtension::PrePostProcessPass_RenderThread(FRDGBuilder& GraphBuilder, const FSceneView& View, const FPostProcessingInputs& Inputs) {
	
}

void FTriangleViewExtension::PreRenderView_RenderThread(FRDGBuilder& GraphBuilder, FSceneView& InView)
{
		
}

void FTriangleViewExtension::SubscribeToPostProcessingPass(EPostProcessingPass Pass, const FSceneView& InView, FAfterPassCallbackDelegateArray& InOutPassCallbacks, bool bIsPassEnabled)
{
	if (Pass == EPostProcessingPass::Tonemap)
	{
		InOutPassCallbacks.Add(FAfterPassCallbackDelegate::CreateRaw(this, &FTriangleViewExtension::TrianglePass_RenderThread));
	}
}

FScreenPassTexture FTriangleViewExtension::TrianglePass_RenderThread(FRDGBuilder& GraphBuilder, const FSceneView& View, const FPostProcessMaterialInputs& InOutInputs)
{

	FScreenPassTexture SceneColor(InOutInputs.GetInput(EPostProcessMaterialInput::SceneColor));

	FScreenPassRenderTarget Output = InOutInputs.OverrideOutput;

	// If the override output is provided, it means that this is the last pass in post processing.
	// You need to do this since we cannot read from SceneColor and write to it (At least it's not supported by UE)
	// OverrideOutput is also required to use so the Pass Sequence in PostProcessing.cpp knows this is the last pass so we don't get a bad texture from Scene Color on the next frame.
	
	if (!Output.IsValid())
	{
		Output = FScreenPassRenderTarget::CreateFromInput(GraphBuilder, SceneColor, View.GetOverwriteLoadAction(), TEXT("OverrideSceneColorTexture"));
	}

	RDG_GPU_STAT_SCOPE(GraphBuilder, TrianglePass)
	RDG_EVENT_SCOPE(GraphBuilder, "TrianglePass");
	
	if (EnumHasAllFlags(SceneColor.Texture->Desc.Flags, TexCreate_ShaderResource) && EnumHasAnyFlags(SceneColor.Texture->Desc.Flags,  TexCreate_RenderTargetable | TexCreate_ResolveTargetable))
	{
		const FIntRect ViewInfo = static_cast<const FViewInfo&>(View).ViewRect;
		const FGlobalShaderMap* ViewShaderMap = static_cast<const FViewInfo&>(View).ShaderMap;
		RenderTriangle(GraphBuilder,ViewShaderMap,ViewInfo,SceneColor);
	}
	
	return MoveTemp(SceneColor);
}

// Draw Triangle Using PixelShaderUtil Helpers
void FTriangleViewExtension::RenderTriangle
(
FRDGBuilder& GraphBuilder,
const FGlobalShaderMap* ViewShaderMap,
const FIntRect& ViewInfo,
const FScreenPassTexture& InSceneColor)
{
	// Begin Setup
	// Shader Parameter Setup
	FTrianglePSParams* PassParams = GraphBuilder.AllocParameters<FTrianglePSParams>();
	PassParams->RenderTargets[0] = FRenderTargetBinding(InSceneColor.Texture, ERenderTargetLoadAction::ELoad);

	// Create Pixel Shader
	TShaderMapRef<FTrianglePS> PixelShader(ViewShaderMap);

	// Add Pass
	AddFullscreenPass<FTrianglePS>(GraphBuilder,
		ViewShaderMap,
		RDG_EVENT_NAME("TranglePass"),
		PixelShader,
		PassParams,
		ViewInfo);
	
}

template <typename TShaderClass>
void FTriangleViewExtension::AddFullscreenPass(
		FRDGBuilder& GraphBuilder,
		const FGlobalShaderMap* GlobalShaderMap,
		FRDGEventName&& PassName,
		const TShaderRef<TShaderClass>& PixelShader,
		typename TShaderClass::FParameters* Parameters,
		const FIntRect& Viewport,
		FRHIBlendState* BlendState,
		FRHIRasterizerState* RasterizerState,
		FRHIDepthStencilState* DepthStencilState,
		uint32 StencilRef)
{
	
	check(PixelShader.IsValid());
	ClearUnusedGraphResources(PixelShader, Parameters);

	GraphBuilder.AddPass(
		Forward<FRDGEventName>(PassName),
		Parameters,
		ERDGPassFlags::Raster,
		[Parameters, GlobalShaderMap, PixelShader, Viewport, BlendState, RasterizerState, DepthStencilState, StencilRef](FRHICommandList& RHICmdList)
	{
			FTriangleViewExtension::DrawFullscreenPixelShader<TShaderClass>(RHICmdList, GlobalShaderMap, PixelShader, *Parameters, Viewport, 
			BlendState, RasterizerState, DepthStencilState, StencilRef);
	});
	
	
}

template <typename TShaderClass>
	void FTriangleViewExtension::DrawFullscreenPixelShader(
		FRHICommandList& RHICmdList, 
		const FGlobalShaderMap* GlobalShaderMap,
		const TShaderRef<TShaderClass>& PixelShader,
		const typename TShaderClass::FParameters& Parameters,
		const FIntRect& Viewport,
		FRHIBlendState* BlendState,
		FRHIRasterizerState* RasterizerState,
		FRHIDepthStencilState* DepthStencilState,
		uint32 StencilRef)
{
	check(PixelShader.IsValid());
	RHICmdList.SetViewport((float)Viewport.Min.X, (float)Viewport.Min.Y, 0.0f, (float)Viewport.Max.X, (float)Viewport.Max.Y, 1.0f);

	// Begin Setup Gpu Pipeline for this Pass
	FGraphicsPipelineStateInitializer GraphicsPSOInit;
	// Begin InitFullScreenPipeline
	TShaderMapRef<FTriangleVS> VertexShader(GlobalShaderMap);
	
	RHICmdList.ApplyCachedRenderTargets(GraphicsPSOInit);
	GraphicsPSOInit.BlendState = TStaticBlendState<>::GetRHI();
	GraphicsPSOInit.RasterizerState = TStaticRasterizerState<>::GetRHI();
	GraphicsPSOInit.DepthStencilState = TStaticDepthStencilState<false, CF_Always>::GetRHI();

	GraphicsPSOInit.BoundShaderState.VertexDeclarationRHI = GTriangleVertexDeclaration.VertexDeclarationRHI;
	GraphicsPSOInit.BoundShaderState.VertexShaderRHI = VertexShader.GetVertexShader();
	GraphicsPSOInit.BoundShaderState.PixelShaderRHI = PixelShader.GetPixelShader();
	GraphicsPSOInit.PrimitiveType = PT_TriangleList;
	
	GraphicsPSOInit.BlendState = BlendState ? BlendState : GraphicsPSOInit.BlendState;
	GraphicsPSOInit.RasterizerState = RasterizerState ? RasterizerState : GraphicsPSOInit.RasterizerState;
	GraphicsPSOInit.DepthStencilState = DepthStencilState ? DepthStencilState : GraphicsPSOInit.DepthStencilState;
	
	SetGraphicsPipelineState(RHICmdList, GraphicsPSOInit, StencilRef);
	SetShaderParameters(RHICmdList, PixelShader, PixelShader.GetPixelShader(), Parameters);
	DrawFullScreenTriangle(RHICmdList, 1);
}

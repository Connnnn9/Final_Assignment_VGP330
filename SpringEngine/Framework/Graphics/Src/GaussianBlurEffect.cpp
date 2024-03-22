#include "Precompiled.h"
#include "GaussianBlurEffect.h"
#include "GraphicsSystem.h"

#include "Camera.h"
#include "GraphicsSystem.h"
#include "RenderObject.h"
#include "VertexTypes.h"

using namespace SpringEngine;
using namespace SpringEngine::Graphics;

void GaussianBlurEffect::Initialize()
{	 
	GraphicsSystem* gs = GraphicsSystem::Get();
	const float screenWidth = gs->GetBackBufferWidth();
	const float screenHeight = gs->GetBackBufferHeight();
	mHorizontalBlurRenderTarget.Initialize(screenWidth, screenHeight, RenderTarget::Format::RGBA_U8);
	mVerticalBlurRenderTarget.Initialize(screenWidth, screenHeight, RenderTarget::Format::RGBA_U8);

	std::filesystem::path shaderFile = "../../Assets/Shaders/GaussianBlur.fx";
	mVertexShader.Initialize<VertexPX>(shaderFile);
	mHorizontalBlurPixelShader.Initialize(shaderFile,"HorizontalBlurPS");
	mVerticalBlurPixelShader.Initialize(shaderFile,"VerticalBlurPS");

	mSettingsBuffer.Initialize();
	mSampler.Initialize(Sampler::Filter::Linear,Sampler::AddressMode::Wrap);
}	 
	 
void GaussianBlurEffect::Terminate()
{	 
	mSampler.Terminate();
	mSettingsBuffer.Terminate();
	mVerticalBlurPixelShader.Terminate();
	mHorizontalBlurPixelShader.Terminate();
	mVertexShader.Terminate();
	mVerticalBlurRenderTarget.Terminate();
	mHorizontalBlurRenderTarget.Terminate();
}	 
	 
void GaussianBlurEffect::Begin()
{	 
	mVertexShader.Bind();
	
	GraphicsSystem* gs = GraphicsSystem::Get();

	SettingsData data;
	data.screenWidth = gs->GetBackBufferWidth();
	data.screenHeight = gs->GetBackBufferHeight();
	data.mutiplier = mBlurSaturation;
	mSettingsBuffer.Update(data);
	mSettingsBuffer.BindPS(0);

	mSampler.BindPS(0);
}	 
	 
void GaussianBlurEffect::End()
{	 
	GraphicsSystem* gs = GraphicsSystem::Get();
	gs->ResetRenderTarget();
	gs->ResetViewport();
}	 
	 
void GaussianBlurEffect::Render(const RenderObject& renderObject)
{	 
	GraphicsSystem* gs = GraphicsSystem::Get();
	mHorizontalBlurRenderTarget.BeginRender();
		mSourceTexture->BindPS(0);
		mHorizontalBlurPixelShader.Bind();
		renderObject.meshBuffer.Render();
		Texture::UnbindPS(0);
	mHorizontalBlurRenderTarget.EndRender();

	for (uint32_t i = 1; i < mBlurIterrations; ++i)
	{
		mVerticalBlurRenderTarget.BeginRender();
			mHorizontalBlurRenderTarget.BindPS(0);
			mVerticalBlurPixelShader.Bind();
			renderObject.meshBuffer.Render();
			Texture::UnbindPS(0);
		mVerticalBlurRenderTarget.EndRender();

		mHorizontalBlurRenderTarget.BeginRender();
			mVerticalBlurRenderTarget.BindPS(0);
			mHorizontalBlurPixelShader.Bind();
			renderObject.meshBuffer.Render();
			Texture::UnbindPS(0);
		mHorizontalBlurRenderTarget.EndRender();
	}

	mVerticalBlurRenderTarget.BeginRender();
		mHorizontalBlurRenderTarget.BindPS(0);
		mVerticalBlurPixelShader.Bind();
		renderObject.meshBuffer.Render();
		Texture::UnbindPS(0);
	mVerticalBlurRenderTarget.EndRender();
}	 
	 
void GaussianBlurEffect::DebugUI()
{	 
	if (ImGui::CollapsingHeader("GaussianBlurEffect",ImGuiTreeNodeFlags_DefaultOpen))
	{
		ImGui::DragInt("BlurIterations", &mBlurIterrations, 1, 1, 100);
		ImGui::DragFloat("BlurSaturation", &mBlurSaturation, 0.001f, 1.0f, 10.0f);
	}
}	 
	 
void GaussianBlurEffect::SetSourceTexture(const Texture& texture)
{
	mSourceTexture = &texture;
}

const Texture& GaussianBlurEffect::GetHorizontalBlurTexture() const
{
	return mHorizontalBlurRenderTarget;
}

const Texture& GaussianBlurEffect::GetVerticalBlurTexture() const
{
	return mVerticalBlurRenderTarget;
}

const Texture& GaussianBlurEffect::GetResultTexture() const
{
	return mVerticalBlurRenderTarget;
}

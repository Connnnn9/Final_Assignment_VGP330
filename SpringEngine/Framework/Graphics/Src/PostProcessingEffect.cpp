#include"Precompiled.h"
#include"PostProcessingEffect.h"

#include "Camera.h"
#include "GraphicsSystem.h"
#include "RenderObject.h"
#include "VertexTypes.h"
#include <filesystem>

using namespace SpringEngine;
using namespace SpringEngine::Graphics;

namespace
{
	const char* gModeNames[] =
	{
			"None",
			"Monochrome",
			"Invert",
			"Mirror",
			"Blur",
			"Combine2",
			"MotionBlur",
			"ChromaticAberration",
			"GradientTransparency"
	};
}

void PostProcessingEffect::Initialize(const std::filesystem::path& filePath)
{
	mVertecShader.Initialize<VertexPX>(filePath);
	mPixelShader.Initialize(filePath);

	mSampler.Initialize(Sampler::Filter::Point, Sampler::AddressMode::Wrap);

	mPostProcessBuffer.Initialize();
}

void PostProcessingEffect::Terminate()
{
	mPostProcessBuffer.Terminate();
	mSampler.Terminate();
	mPixelShader.Terminate();
	mVertecShader.Terminate();
}

void PostProcessingEffect::Begin()
{
	mVertecShader.Bind();
	mPixelShader.Bind();
	mSampler.BindPS(0);

	for (uint32_t i = 0; i < mTextures.size(); ++i)
	{
		if (mTextures[i] != nullptr)
		{
			mTextures[i]->BindPS(i);
		}
	}
	PostProcessingData data;
	data.mode = static_cast<int>(mMode);

	switch (mMode)
	{
	case Mode::None: break;
	case Mode::Monochrome: break;
	case Mode::Invert: break;
	case Mode::Mirror:
	{
		data.params0 = mMirrorScaleX;
		data.params1 = mMirrorScaleY;
	}
	break;
	case Mode::Blur:
	{
		GraphicsSystem* gs = GraphicsSystem::Get();
		const float screenHeight = gs->GetBackBufferHeight();
		const float screenWidth = gs->GetBackBufferWidth();
		data.params0 = mBlurStrenth / screenWidth;
		data.params1 = mBlurStrenth / screenHeight;
	}
	break;
	case Mode::Combine2: break;
		break;
	case Mode::MotionBlur:
	{
		GraphicsSystem* gs = GraphicsSystem::Get();
		const float screenHeight = gs->GetBackBufferHeight();
		const float screenWidth = gs->GetBackBufferWidth();
		data.params0 = mBlurStrenth / screenWidth;
		data.params1 = mBlurStrenth / screenHeight;
	}
	break;
	case Mode::ChromaticAberration:
	{
		data.params0 = mAberrationValue;
	}
	break;
	case Mode::GradientTransparency:
	{
		
		data.params2 = mGradientTransparencyStrength;
	}
	break;
	default:
		break;
	}

	mPostProcessBuffer.Update(data);
	mPostProcessBuffer.BindPS(0);
}

void PostProcessingEffect::End()
{
	for (uint32_t i = 0; i < mTextures.size(); ++i)
	{
		Texture::UnbindPS(i);
	}
}

void PostProcessingEffect::Render(const RenderObject& renderObject)
{
	renderObject.meshBuffer.Render();
}

void PostProcessingEffect::SetTexture(const Texture* texture, uint32_t slot)
{
	ASSERT(slot < mTextures.size(), "PostProcessingEffect: invalid slot index");
	mTextures[slot] = texture;
}

void PostProcessingEffect::SetMode(Mode mode)
{
	mMode = mode;
}
void PostProcessingEffect::SetGradientTransparencyStrength(float strength)
{
	PostProcessingData data;
	data.mode = static_cast<int>(mMode);
	data.params0 = 0.0f; 
	data.params1 = 0.0f;
	data.params2 = strength;
	mPostProcessBuffer.Update(data);

}

void PostProcessingEffect::Debug()
{
	if (ImGui::CollapsingHeader("PostProcessingEffect", ImGuiTreeNodeFlags_DefaultOpen))
	{
		int currentMode = static_cast<int>(mMode);
		if (ImGui::Combo("Mode", &currentMode, gModeNames, std::size(gModeNames)))
		{
			mMode = static_cast<Mode>(currentMode);
		}
		if (mMode == Mode::Mirror)
		{
			ImGui::DragFloat("MirrorScaleX", &mMirrorScaleX, 0.1f, -1.0f, 1.0f);
			ImGui::DragFloat("MirrorScaleY", &mMirrorScaleX, 0.1f, -1.0f, 1.0f);
		}
		else if (mMode == Mode::Blur|| mMode==Mode::MotionBlur)
		{
			ImGui::DragFloat("BlurStrength", &mBlurStrenth, 1.0f, 0.0f, 100.0f);
		}
		else if (mMode == Mode::ChromaticAberration)
		{
			ImGui::DragFloat("AberrationValue", &mAberrationValue, 0.001f, 0.001f, 1.0f);
		}
		else if(mMode == Mode::GradientTransparency)
		{
			ImGui::SliderFloat("Gradient Transparency Strength", &mGradientTransparencyStrength, 0.0f, 1.0f, "Strength: %.3f");
		}
	}
}

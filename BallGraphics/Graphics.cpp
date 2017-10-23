#include "Graphics.h"

namespace BallGraphics
{

Graphics::Graphics()
{
}

void Graphics::initialize(int screenWidth,
                          int screenHeight,
                          bool vsync,
                          HWND hwnd,
                          bool fullScreen)
{
    screenWidth_ = screenWidth;
    screenHeight_ = screenHeight;
    d3d_.initialize(screenWidth, screenHeight, vsync, hwnd, fullScreen);
    d3d_.turnOnAlphaBlending();
}

void Graphics::frame()
{
    d3d_.beginScene();

	for (auto& camera : cameras_)
	{
		camera.second->render();
	}

	for (auto& layer : layers_)
	{
		d3d_.enableZBuffer(layer.first == 0);
		for (auto& instId : layer.second)
		{
			auto instIt = meshInstances_.find(instId);
			assert(instIt != meshInstances_.end());
			auto meshIt = meshes_.find(instIt->second.meshId);
			assert(meshIt != meshes_.end());
			meshIt->second.mesh->render(instIt->second.worldFunc, instIt->second.camera);
		}
		
	}

    d3d_.endScene();
}

void Graphics::shutdown()
{
	for (auto& eff : effects_)
	{
		eff.second->shutdown();
	}
    d3d_.shutdown();
}

Texture Graphics::createTexture(const std::string & fileName) noexcept
{
	Texture result;
	bool init = result.Initialize(d3d_.getDevice(), d3d_.getDeviceContext(), fileName.c_str());
	assert(init);
	return result;
}

Id Graphics::addEffect(Effect* effect) noexcept
{
	auto it = effects_.emplace(nextEffectId_, effect);
	assert(it.second);
	it.first->second->initialize(d3d_);
	return nextEffectId_++;
}

Id Graphics::addMesh(std::unique_ptr<Mesh>&& mesh) noexcept
{
	Mesh_ meshStruct;
	meshStruct.mesh = std::move(mesh);
	auto it = meshes_.emplace(nextMeshId_, std::move(meshStruct));
	assert(it.second);
	return nextMeshId_++;
}

const std::unique_ptr<Mesh>& Graphics::adjustMesh(Id id) noexcept
{
	auto it = meshes_.find(id);
	assert(it != meshes_.end());
	return it->second.mesh;
}

Id Graphics::addMeshInstance(MeshInstance && instance) noexcept
{
	assert(instance.camera);
	assert(instance.worldFunc);
	auto meshIt = meshes_.find(instance.meshId);
	assert(meshIt != meshes_.end());
	meshIt->second.usages++;

	auto it = meshInstances_.emplace(nextMeshInstanceId_, std::move(instance));
	assert(it.second);
	auto layerIt = layers_[it.first->second.layer].emplace(nextMeshInstanceId_);
	assert(layerIt.second);
	return nextMeshInstanceId_++;
}

Id Graphics::addCamera(std::shared_ptr<Camera> camera) noexcept
{
	auto it = cameras_.emplace(nextCameraId_, std::move(camera));
	assert(it.second);
	return nextCameraId_++;
}

void Graphics::removeMesh(Id id) noexcept
{
	auto it = meshes_.find(id);
	assert(it != meshes_.end());
	assert(it->second.usages == 0);
	meshes_.erase(it);
}

void Graphics::removeMeshInstance(Id id) noexcept
{
	auto it = meshInstances_.find(id);
	assert(it != meshInstances_.end());

	auto meshIt = meshes_.find(it->second.meshId);
	assert(meshIt != meshes_.end());
	assert(meshIt->second.usages > 0);
	meshIt->second.usages--;

	auto layerIt = layers_.find(it->second.layer);
	assert(layerIt != layers_.end());
	auto layerNum = layerIt->second.erase(id);
	assert(layerNum == 1);
	meshInstances_.erase(it);
}

int Graphics::getScreenWidth() const noexcept
{
    return screenWidth_;
}

int Graphics::getScreenHeight() const noexcept
{
    return screenHeight_;
}

}//BallGraphics
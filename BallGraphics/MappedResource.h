#pragma once
#include <d3d11.h>

namespace BallGraphics
{

template<typename T>
class MappedResource
{
public:
	MappedResource(ID3D11DeviceContext*, ID3D11Resource*) noexcept;
	~MappedResource() noexcept;
	T* operator->() const noexcept;
	T& operator *() noexcept;
private:
	D3D11_MAPPED_SUBRESOURCE mappedResource_;
	ID3D11DeviceContext* deviceContext_;
	ID3D11Resource* resource_;
};

template<typename T>
inline MappedResource<T>::MappedResource(ID3D11DeviceContext* deviceContext, ID3D11Resource* resource) noexcept:
	deviceContext_{ deviceContext },
	resource_{ resource }
{
	HRESULT result = deviceContext_->Map(resource_, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource_);
	assert(result == S_OK);
}

template<typename T>
inline MappedResource<T>::~MappedResource() noexcept
{
	deviceContext_->Unmap(resource_, 0);
}

template<typename T>
inline T * MappedResource<T>::operator->() const noexcept
{
	return (T*)mappedResource_.pData;
}

template<typename T>
inline T & MappedResource<T>::operator*() noexcept
{
	return *(T*)mappedResource_.pData;
}

}//namespace BallGraphics
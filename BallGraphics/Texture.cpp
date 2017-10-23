#include "Texture.h"
#include <stdio.h>
#include <assert.h>

using namespace BallUtils;

namespace BallGraphics
{

Texture::Texture(){}

Texture::~Texture(){}

ComPtr<ID3D11Texture2D> Texture::create_texture_2d(ID3D11Device* device, const D3D11_TEXTURE2D_DESC & desc){
    ID3D11Texture2D* tmp;
    HRESULT result = device->CreateTexture2D(&desc, NULL, &tmp);
    assert(result == S_OK);
    return ComPtr<ID3D11Texture2D>(tmp);
}

ComPtr<ID3D11ShaderResourceView> Texture::create_shader_resource_view(ID3D11Device * device,
                                                                      ID3D11Resource * resource,
                                                                      const D3D11_SHADER_RESOURCE_VIEW_DESC & desc){
    ID3D11ShaderResourceView* tmp;
    HRESULT result = device->CreateShaderResourceView(resource, &desc, &tmp);
    assert(result == S_OK);
    return ComPtr<ID3D11ShaderResourceView>(tmp);
}

bool Texture::Initialize(ID3D11Device* device, ID3D11DeviceContext* deviceContext, const char* filename){
    int height, width;
    D3D11_TEXTURE2D_DESC texture_desc;
    unsigned int rowPitch;
    D3D11_SHADER_RESOURCE_VIEW_DESC srv_desc;
    // Load the targa image data into memory.
    LoadTarga(filename, height, width);

    // Setup the description of the texture.
    texture_desc.Height = height;
    texture_desc.Width = width;
    texture_desc.MipLevels = 0;
    texture_desc.ArraySize = 1;
    texture_desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    texture_desc.SampleDesc.Count = 1;
    texture_desc.SampleDesc.Quality = 0;
    texture_desc.Usage = D3D11_USAGE_DEFAULT;
    texture_desc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;
    texture_desc.CPUAccessFlags = 0;
    texture_desc.MiscFlags = D3D11_RESOURCE_MISC_GENERATE_MIPS;

    texture_ = create_texture_2d(device, texture_desc);

    // Set the row pitch of the targa image data.
    rowPitch = (width * 4) * sizeof(unsigned char);
    // Copy the targa image data into the texture.
    deviceContext->UpdateSubresource(texture_, 0, NULL, m_targa_data.data(), rowPitch, 0);
    // Setup the shader resource view description.
    srv_desc.Format = texture_desc.Format;
    srv_desc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
    srv_desc.Texture2D.MostDetailedMip = 0;
    srv_desc.Texture2D.MipLevels = -1;

    // Create the shader resource view for the texture.
    m_texture_view = create_shader_resource_view(device, texture_, srv_desc);

    // Generate mipmaps for this texture.
    deviceContext->GenerateMips(m_texture_view);

    // Release the targa image data now that the image data has been loaded into the texture.
    

    return true;
}

void Texture::Shutdown(){}

ID3D11ShaderResourceView* Texture::GetTexture(){
    return m_texture_view;
}

void Texture::LoadTarga(const char* filename, int& height, int& width){
    int error, bpp, imageSize, index, i, j, k;
    FILE* filePtr;
    unsigned int count;
    TargaHeader targaFileHeader;
    unsigned char* targaImage;


    // Open the targa file for reading in binary.
    error = fopen_s(&filePtr, filename, "rb");
    assert(error == 0);

    // Read in the file header.
    count = (unsigned int)fread(&targaFileHeader, sizeof(TargaHeader), 1, filePtr);
    assert(count == 1);

    // Get the important information from the header.
    height = (int)targaFileHeader.height;
    width = (int)targaFileHeader.width;
    bpp = (int)targaFileHeader.bpp;

    // Check that it is 32 bit and not 24 bit.
    assert(bpp == 32);

    // Calculate the size of the 32 bit image data.
    imageSize = width * height * 4;

    // Allocate memory for the targa image data.

    targaImage = new unsigned char[imageSize];
    assert(targaImage);

    // Read in the targa image data.
    count = (unsigned int)fread(targaImage, 1, imageSize, filePtr);
    assert(count == imageSize);

    // Close the file.
    error = fclose(filePtr);
    assert(error == 0);

    // Allocate memory for the targa destination data.
    m_targa_data.resize(imageSize);

    // Initialize the index into the targa destination data array.
    index = 0;

    // Initialize the index into the targa image data.
    k = (width * height * 4) - (width * 4);

    // Now copy the targa image data into the targa destination array in the correct order since the targa format is stored upside down.
    for(j = 0; j<height; j++){
        for(i = 0; i<width; i++){
            m_targa_data[index + 0] = targaImage[k + 2];  // Red.
            m_targa_data[index + 1] = targaImage[k + 1];  // Green.
            m_targa_data[index + 2] = targaImage[k + 0];  // Blue
            m_targa_data[index + 3] = targaImage[k + 3];  // Alpha

                                                         // Increment the indexes into the targa data.
            k += 4;
            index += 4;
        }

        // Set the targa image data index back to the preceding row at the beginning of the column since its reading it in upside down.
        k -= (width * 8);
    }

    // Release the targa image data now that it was copied into the destination array.
    delete[] targaImage;
    targaImage = 0;
}

}//BallGraphics
#include "Texture.h"
#include <stdio.h>
#include <assert.h>

using namespace BallUtils;

namespace BallGraphics
{

Texture::Texture(){}

Texture::~Texture(){}

ComPtr<ID3D11Texture2D> Texture::createTexture2d( ID3D11Device* device, const D3D11_TEXTURE2D_DESC & desc )
{
    ID3D11Texture2D* tmp;
    HRESULT result = device->CreateTexture2D( &desc, NULL, &tmp );
    assert( result == S_OK );
    return ComPtr<ID3D11Texture2D>( tmp );
}

ComPtr<ID3D11ShaderResourceView> Texture::createShaderResourceView(
    ID3D11Device * device,
    ID3D11Resource * resource,
    const D3D11_SHADER_RESOURCE_VIEW_DESC & desc )
{
    ID3D11ShaderResourceView* tmp;
    HRESULT result = device->CreateShaderResourceView( resource, &desc, &tmp );
    assert( result == S_OK );
    return ComPtr<ID3D11ShaderResourceView>( tmp );
}

bool Texture::Initialize( ID3D11Device* device, ID3D11DeviceContext* deviceContext, const char* filename )
{
    int height, width;
    D3D11_TEXTURE2D_DESC textureDesc;
    unsigned int rowPitch;
    // Load the targa image data into memory.
    LoadTarga( filename, height, width );

    // Setup the description of the texture.
    textureDesc.Height = height;
    textureDesc.Width = width;
    textureDesc.MipLevels = 0;
    textureDesc.ArraySize = 1;
    textureDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    textureDesc.SampleDesc.Count = 1;
    textureDesc.SampleDesc.Quality = 0;
    textureDesc.Usage = D3D11_USAGE_DEFAULT;
    textureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;
    textureDesc.CPUAccessFlags = 0;
    textureDesc.MiscFlags = D3D11_RESOURCE_MISC_GENERATE_MIPS;

    texture_ = createTexture2d( device, textureDesc );

    // Set the row pitch of the targa image data.
    rowPitch = ( width * 4 ) * sizeof( unsigned char );
    // Copy the targa image data into the texture.
    deviceContext->UpdateSubresource( texture_, 0, NULL, targaData_.data(), rowPitch, 0 );

    // Setup the shader resource view description.
    D3D11_SHADER_RESOURCE_VIEW_DESC shaderResourceViewDesc;
    shaderResourceViewDesc.Format = textureDesc.Format;
    shaderResourceViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
    shaderResourceViewDesc.Texture2D.MostDetailedMip = 0;
    shaderResourceViewDesc.Texture2D.MipLevels = -1;

    // Create the shader resource view for the texture.
    textureView_ = createShaderResourceView( device, texture_, shaderResourceViewDesc );

    // Generate mipmaps for this texture.
    deviceContext->GenerateMips( textureView_ );

    return true;
}

void Texture::Shutdown()
{
}

ID3D11ShaderResourceView* Texture::GetTexture()
{
    return textureView_;
}

void Texture::LoadTarga( const char* filename, int& height, int& width )
{
    int error, bpp, imageSize, index, i, j, k;
    FILE* filePtr;
    unsigned int count;
    TargaHeader targaFileHeader;
    unsigned char* targaImage;


    // Open the targa file for reading in binary.
    error = fopen_s( &filePtr, filename, "rb" );
    assert( error == 0 );

    // Read in the file header.
    count = (unsigned int) fread( &targaFileHeader, sizeof( TargaHeader ), 1, filePtr );
    assert( count == 1 );

    // Get the important information from the header.
    height = (int) targaFileHeader.height;
    width = (int) targaFileHeader.width;
    bpp = (int) targaFileHeader.bpp;

    // Check that it is 32 bit and not 24 bit.
    assert( bpp == 32 );

    // Calculate the size of the 32 bit image data.
    imageSize = width * height * 4;

    // Allocate memory for the targa image data.

    targaImage = new unsigned char[imageSize];
    assert( targaImage );

    // Read in the targa image data.
    count = (unsigned int) fread( targaImage, 1, imageSize, filePtr );
    assert( count == imageSize );

    // Close the file.
    error = fclose( filePtr );
    assert( error == 0 );

    // Allocate memory for the targa destination data.
    targaData_.resize( imageSize );

    // Initialize the index into the targa destination data array.
    index = 0;

    // Initialize the index into the targa image data.
    k = ( width * height * 4 ) - ( width * 4 );

    // Now copy the targa image data into the targa destination array in the correct order since the targa format is stored upside down.
    for ( j = 0; j<height; j++ )
    {
        for ( i = 0; i<width; i++ )
        {
            targaData_[index + 0] = targaImage[k + 2];  // Red.
            targaData_[index + 1] = targaImage[k + 1];  // Green.
            targaData_[index + 2] = targaImage[k + 0];  // Blue
            targaData_[index + 3] = targaImage[k + 3];  // Alpha

                                                        // Increment the indexes into the targa data.
            k += 4;
            index += 4;
        }

        // Set the targa image data index back to the preceding row at the beginning of the column since its reading it in upside down.
        k -= ( width * 8 );
    }

    // Release the targa image data now that it was copied into the destination array.
    delete[] targaImage;
    targaImage = 0;
}

} //namespace
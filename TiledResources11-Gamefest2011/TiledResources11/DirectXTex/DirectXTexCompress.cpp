//-------------------------------------------------------------------------------------
// DirectXTexCompress.cpp
//  
// DirectX Texture Library - Texture compression
//
// THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
// THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
// PARTICULAR PURPOSE.
//
// Copyright (c) Microsoft Corporation. All rights reserved.
//-------------------------------------------------------------------------------------

#include <windows.h>
#include <malloc.h>
#include <assert.h>
#include <memory>

#ifdef _OPENMP
#include <omp.h>
#pragma warning(disable : 4616 6001 6993)
#endif

#include "bc.h"
#include "scoped.h"

#include "directxtexp.h"

namespace DirectX
{

inline static DWORD _GetBCFlags( _In_ DWORD compress )
{
    static_assert( TEX_COMPRESS_RGB_DITHER == BC_FLAGS_DITHER_RGB, "TEX_COMPRESS_* flags should match BC_FLAGS_*" );
    static_assert( TEX_COMPRESS_A_DITHER == BC_FLAGS_DITHER_A, "TEX_COMPRESS_* flags should match BC_FLAGS_*"  );
    static_assert( TEX_COMPRESS_DITHER == (BC_FLAGS_DITHER_RGB | BC_FLAGS_DITHER_A), "TEX_COMPRESS_* flags should match BC_FLAGS_*"  );
    static_assert( TEX_COMPRESS_UNIFORM == BC_FLAGS_UNIFORM, "TEX_COMPRESS_* flags should match BC_FLAGS_*"  );
    return ( compress & (BC_FLAGS_DITHER_RGB|BC_FLAGS_DITHER_A|BC_FLAGS_UNIFORM) );
}


//-------------------------------------------------------------------------------------
static HRESULT _CompressBC( _In_ const Image& image, _In_ const Image& result, _In_ DWORD bcflags,
                            _In_ float alphaRef, _In_ bool degenerate )
{
    if ( !image.pixels || !result.pixels )
        return E_POINTER;

    assert( image.width == result.width );
    assert( image.height == result.height );

    const DXGI_FORMAT format = image.format;
    size_t sbpp = BitsPerPixel( format );
    if ( !sbpp )
        return E_FAIL;

    if ( sbpp < 8 )
    {
        // We don't support compressing from monochrome (DXGI_FORMAT_R1_UNORM)
        return HRESULT_FROM_WIN32( ERROR_NOT_SUPPORTED );
    }

    // Round to bytes
    sbpp = ( sbpp + 7 ) / 8;

    uint8_t *pDest = result.pixels;

    // Determine BC format encoder
    BC_ENCODE pfEncode;
    size_t blocksize;
    switch(result.format)
    {
    case DXGI_FORMAT_BC1_UNORM:
    case DXGI_FORMAT_BC1_UNORM_SRGB:    pfEncode = nullptr;         blocksize = 8;   break;
    case DXGI_FORMAT_BC2_UNORM:
    case DXGI_FORMAT_BC2_UNORM_SRGB:    pfEncode = D3DXEncodeBC2;   blocksize = 16;  break;
    case DXGI_FORMAT_BC3_UNORM:
    case DXGI_FORMAT_BC3_UNORM_SRGB:    pfEncode = D3DXEncodeBC3;   blocksize = 16;  break;
    case DXGI_FORMAT_BC4_UNORM:         pfEncode = D3DXEncodeBC4U;  blocksize = 8;   break;
    case DXGI_FORMAT_BC4_SNORM:         pfEncode = D3DXEncodeBC4S;  blocksize = 8;   break;
    case DXGI_FORMAT_BC5_UNORM:         pfEncode = D3DXEncodeBC5U;  blocksize = 16;  break;
    case DXGI_FORMAT_BC5_SNORM:         pfEncode = D3DXEncodeBC5S;  blocksize = 16;  break;
    case DXGI_FORMAT_BC6H_UF16:         pfEncode = D3DXEncodeBC6HU; blocksize = 16;  break;
    case DXGI_FORMAT_BC6H_SF16:         pfEncode = D3DXEncodeBC6HS; blocksize = 16;  break;
    case DXGI_FORMAT_BC7_UNORM:
    case DXGI_FORMAT_BC7_UNORM_SRGB:    pfEncode = D3DXEncodeBC7;   blocksize = 16;  break;
    default:
        return HRESULT_FROM_WIN32( ERROR_NOT_SUPPORTED );
    }

    XMVECTOR temp[16];
    const uint8_t *pSrc = image.pixels;
    const size_t rowPitch = image.rowPitch;
    for( size_t h=0; h < image.height; h += 4 )
    {
        const uint8_t *sptr = pSrc;
        uint8_t* dptr = pDest;
        for( size_t count = 0; count < rowPitch; count += sbpp*4 )
        {
            if ( !_LoadScanline( &temp[0], 4, sptr, rowPitch, format ) )
                return E_FAIL;

            if ( image.height > 1 )
            {
                if ( !_LoadScanline( &temp[4], 4, sptr + rowPitch, rowPitch, format ) )
                    return E_FAIL;

                if ( image.height > 2 )
                {
                    if ( !_LoadScanline( &temp[8], 4, sptr + rowPitch*2, rowPitch, format ) )
                        return E_FAIL;

                    if ( !_LoadScanline( &temp[12], 4, sptr + rowPitch*3, rowPitch, format ) )
                        return E_FAIL;
                }
            }

            if ( degenerate )
            {
                assert( image.width < 4 || image.height < 4 );
                const size_t uSrc[] = { 0, 0, 0, 1 };

                if ( image.width < 4 )
                {
                    for( size_t t=0; t < image.height && t < 4; ++t )
                    {
                        for( size_t s = image.width; s < 4; ++s )
                        {
                            temp[ t*4 + s ] = temp[ t*4 + uSrc[s] ]; 
                        }
                    }
                }

                if ( image.height < 4 )
                {
                    for( size_t t=image.height; t < 4; ++t )
                    {
                        for( size_t s =0; s < 4; ++s )
                        {
                            temp[ t*4 + s ] = temp[ uSrc[t]*4 + s ]; 
                        }
                    }
                }
            }

            _ConvertScanline( temp, 16, result.format, format, 0 );
            
            if ( pfEncode )
                pfEncode( dptr, temp, bcflags );
            else
                D3DXEncodeBC1( dptr, temp, alphaRef, bcflags );

            sptr += sbpp*4;
            dptr += blocksize;
        }

        pSrc += rowPitch*4;
        pDest += result.rowPitch;
    }

    return S_OK;
}


//-------------------------------------------------------------------------------------
#ifdef _OPENMP
static HRESULT _CompressBC_Parallel( _In_ const Image& image, _In_ const Image& result, _In_ DWORD bcflags,
                                     _In_ float alphaRef )
{
    if ( !image.pixels || !result.pixels )
        return E_POINTER;

    // Parallel version doesn't support degenerate case
    assert( ((image.width % 4) == 0) && ((image.height % 4) == 0 ) );

    assert( image.width == result.width );
    assert( image.height == result.height );

    const DXGI_FORMAT format = image.format;
    size_t sbpp = BitsPerPixel( format );
    if ( !sbpp )
        return E_FAIL;

    if ( sbpp < 8 )
    {
        // We don't support compressing from monochrome (DXGI_FORMAT_R1_UNORM)
        return HRESULT_FROM_WIN32( ERROR_NOT_SUPPORTED );
    }

    // Round to bytes
    sbpp = ( sbpp + 7 ) / 8;

    // Determine BC format encoder
    BC_ENCODE pfEncode;
    size_t blocksize;
    switch(result.format)
    {
    case DXGI_FORMAT_BC1_UNORM:
    case DXGI_FORMAT_BC1_UNORM_SRGB:    pfEncode = nullptr;         blocksize = 8;   break;
    case DXGI_FORMAT_BC2_UNORM:
    case DXGI_FORMAT_BC2_UNORM_SRGB:    pfEncode = D3DXEncodeBC2;   blocksize = 16;  break;
    case DXGI_FORMAT_BC3_UNORM:
    case DXGI_FORMAT_BC3_UNORM_SRGB:    pfEncode = D3DXEncodeBC3;   blocksize = 16;  break;
    case DXGI_FORMAT_BC4_UNORM:         pfEncode = D3DXEncodeBC4U;  blocksize = 8;   break;
    case DXGI_FORMAT_BC4_SNORM:         pfEncode = D3DXEncodeBC4S;  blocksize = 8;   break;
    case DXGI_FORMAT_BC5_UNORM:         pfEncode = D3DXEncodeBC5U;  blocksize = 16;  break;
    case DXGI_FORMAT_BC5_SNORM:         pfEncode = D3DXEncodeBC5S;  blocksize = 16;  break;
    case DXGI_FORMAT_BC6H_UF16:         pfEncode = D3DXEncodeBC6HU; blocksize = 16;  break;
    case DXGI_FORMAT_BC6H_SF16:         pfEncode = D3DXEncodeBC6HS; blocksize = 16;  break;
    case DXGI_FORMAT_BC7_UNORM:
    case DXGI_FORMAT_BC7_UNORM_SRGB:    pfEncode = D3DXEncodeBC7;   blocksize = 16;  break;
    default:
        return HRESULT_FROM_WIN32( ERROR_NOT_SUPPORTED );
    }

    // Refactored version of loop to support parallel independance
    const size_t nBlocks = max( 1, image.width / 4) * max( 1, image.height / 4);

    bool fail = false;

#pragma omp parallel for
    for( int nb=0; nb < static_cast<int>( nBlocks ); ++nb )
    {
        const size_t nbWidth = max( 1, image.width / 4);

        const size_t y = nb / nbWidth;
        const size_t x = nb - (y*nbWidth);

        assert( x < image.width && y < image.height );

        size_t rowPitch = image.rowPitch;
        const uint8_t *pSrc = image.pixels + (y*4*rowPitch) + (x*4*sbpp);

        uint8_t *pDest = result.pixels + (nb*blocksize);

        XMVECTOR temp[16];
        if ( !_LoadScanline( &temp[0], 4, pSrc, rowPitch, format ) )
            fail = true;

        if ( !_LoadScanline( &temp[4], 4, pSrc + rowPitch, rowPitch, format ) )
            fail = true;

        if ( !_LoadScanline( &temp[8], 4, pSrc + rowPitch*2, rowPitch, format ) )
            fail = true;

        if ( !_LoadScanline( &temp[12], 4, pSrc + rowPitch*3, rowPitch, format ) )
            fail = true;

        _ConvertScanline( temp, 16, result.format, format, 0 );
            
        if ( pfEncode )
            pfEncode( pDest, temp, bcflags );
        else
            D3DXEncodeBC1( pDest, temp, alphaRef, bcflags );
    }

    return (fail) ? E_FAIL : S_OK;
}

#endif // _OPENMP


//-------------------------------------------------------------------------------------
static DXGI_FORMAT _DefaultDecompress( _In_ DXGI_FORMAT format )
{
    switch( format )
    {
    case DXGI_FORMAT_BC1_TYPELESS:
    case DXGI_FORMAT_BC1_UNORM:
    case DXGI_FORMAT_BC2_TYPELESS:
    case DXGI_FORMAT_BC2_UNORM:
    case DXGI_FORMAT_BC3_TYPELESS:
    case DXGI_FORMAT_BC3_UNORM:
    case DXGI_FORMAT_BC7_TYPELESS:
    case DXGI_FORMAT_BC7_UNORM:
        return DXGI_FORMAT_R8G8B8A8_UNORM;

    case DXGI_FORMAT_BC1_UNORM_SRGB:
    case DXGI_FORMAT_BC2_UNORM_SRGB:
    case DXGI_FORMAT_BC3_UNORM_SRGB:
    case DXGI_FORMAT_BC7_UNORM_SRGB:
        return DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;

    case DXGI_FORMAT_BC4_TYPELESS:
    case DXGI_FORMAT_BC4_UNORM:
        return DXGI_FORMAT_R8_UNORM;

    case DXGI_FORMAT_BC4_SNORM:
        return DXGI_FORMAT_R8_SNORM;

    case DXGI_FORMAT_BC5_TYPELESS:
    case DXGI_FORMAT_BC5_UNORM:
        return DXGI_FORMAT_R8G8_UNORM;

    case DXGI_FORMAT_BC5_SNORM:
        return DXGI_FORMAT_R8G8_SNORM;

    case DXGI_FORMAT_BC6H_TYPELESS:
    case DXGI_FORMAT_BC6H_UF16:
    case DXGI_FORMAT_BC6H_SF16:
        // We could use DXGI_FORMAT_R32G32B32_FLOAT here since BC6H is always Alpha 1.0,
        // but this format is more supported by viewers
        return DXGI_FORMAT_R32G32B32A32_FLOAT;

    default:
        return DXGI_FORMAT_UNKNOWN;
    }
}


//-------------------------------------------------------------------------------------
static HRESULT _DecompressBC( _In_ const Image& cImage, _In_ const Image& result )
{
    if ( !cImage.pixels || !result.pixels )
        return E_POINTER;

    assert( cImage.width == result.width );
    assert( cImage.height == result.height );

    // Image must be a multiple of 4 (degenerate cases of 1x1, 1x2, 2x1, and 2x2 are allowed)
    size_t width = cImage.width;
    if ( (width % 4) != 0 )
    {
        if ( width != 1 && width != 2 )
            return E_INVALIDARG;
    }

    size_t height = cImage.height;
    if ( (height % 4) != 0 )
    {
        if ( height != 1 && height != 2 )
            return E_INVALIDARG;
    }

    const DXGI_FORMAT format = result.format;
    size_t dbpp = BitsPerPixel( format );
    if ( !dbpp )
        return E_FAIL;

    if ( dbpp < 8 )
    {
        // We don't support decompressing to monochrome (DXGI_FORMAT_R1_UNORM)
        return HRESULT_FROM_WIN32( ERROR_NOT_SUPPORTED );
    }

    // Round to bytes
    dbpp = ( dbpp + 7 ) / 8;

    uint8_t *pDest = result.pixels;
    if ( !pDest )
        return E_POINTER;

    // Promote "typeless" BC formats
    DXGI_FORMAT cformat;
    switch( cImage.format )
    {
    case DXGI_FORMAT_BC1_TYPELESS:  cformat = DXGI_FORMAT_BC1_UNORM; break;
    case DXGI_FORMAT_BC2_TYPELESS:  cformat = DXGI_FORMAT_BC2_UNORM; break;
    case DXGI_FORMAT_BC3_TYPELESS:  cformat = DXGI_FORMAT_BC3_UNORM; break;
    case DXGI_FORMAT_BC4_TYPELESS:  cformat = DXGI_FORMAT_BC4_UNORM; break;
    case DXGI_FORMAT_BC5_TYPELESS:  cformat = DXGI_FORMAT_BC5_UNORM; break;
    case DXGI_FORMAT_BC6H_TYPELESS: cformat = DXGI_FORMAT_BC6H_UF16; break;
    case DXGI_FORMAT_BC7_TYPELESS:  cformat = DXGI_FORMAT_BC7_UNORM; break;
    default:                        cformat = cImage.format;         break;
    }

    // Determine BC format decoder
    BC_DECODE pfDecode;
    size_t sbpp;
    switch(cformat)
    {
    case DXGI_FORMAT_BC1_UNORM:
    case DXGI_FORMAT_BC1_UNORM_SRGB:    pfDecode = D3DXDecodeBC1;   sbpp = 8;   break;
    case DXGI_FORMAT_BC2_UNORM:
    case DXGI_FORMAT_BC2_UNORM_SRGB:    pfDecode = D3DXDecodeBC2;   sbpp = 16;  break;
    case DXGI_FORMAT_BC3_UNORM:
    case DXGI_FORMAT_BC3_UNORM_SRGB:    pfDecode = D3DXDecodeBC3;   sbpp = 16;  break;
    case DXGI_FORMAT_BC4_UNORM:         pfDecode = D3DXDecodeBC4U;  sbpp = 8;   break;
    case DXGI_FORMAT_BC4_SNORM:         pfDecode = D3DXDecodeBC4S;  sbpp = 8;   break;
    case DXGI_FORMAT_BC5_UNORM:         pfDecode = D3DXDecodeBC5U;  sbpp = 16;  break;
    case DXGI_FORMAT_BC5_SNORM:         pfDecode = D3DXDecodeBC5S;  sbpp = 16;  break;
    case DXGI_FORMAT_BC6H_UF16:         pfDecode = D3DXDecodeBC6HU; sbpp = 16;  break;
    case DXGI_FORMAT_BC6H_SF16:         pfDecode = D3DXDecodeBC6HS; sbpp = 16;  break;
    case DXGI_FORMAT_BC7_UNORM:
    case DXGI_FORMAT_BC7_UNORM_SRGB:    pfDecode = D3DXDecodeBC7;   sbpp = 16;  break;
    default:
        return HRESULT_FROM_WIN32( ERROR_NOT_SUPPORTED );
    }

    XMVECTOR temp[16];
    const uint8_t *pSrc = cImage.pixels;
    const size_t rowPitch = result.rowPitch;
    for( size_t h=0; h < cImage.height; h += 4 )
    {
        const uint8_t *sptr = pSrc;
        uint8_t* dptr = pDest;
        for( size_t count = 0; count < cImage.rowPitch; count += sbpp )
        {
            pfDecode( temp, sptr );
            _ConvertScanline( temp, 16, format, cformat, 0 );

            if ( !_StoreScanline( dptr, rowPitch, format, &temp[0], 4 ) )
                return E_FAIL;

            if ( result.height > 1 )
            {
                if ( !_StoreScanline( dptr + rowPitch, rowPitch, format, &temp[4], 4 ) )
                    return E_FAIL;

                if ( result.height > 2 )
                {
                    if ( !_StoreScanline( dptr + rowPitch*2, rowPitch, format, &temp[8], 4 ) )
                        return E_FAIL;

                    if ( !_StoreScanline( dptr + rowPitch*3, rowPitch, format, &temp[12], 4 ) )
                        return E_FAIL;
                }
            }

            sptr += sbpp;
            dptr += dbpp*4;
        }

        pSrc += cImage.rowPitch;
        pDest += rowPitch*4;
    }

    return S_OK;
}


//=====================================================================================
// Entry-points
//=====================================================================================

//-------------------------------------------------------------------------------------
// Compression
//-------------------------------------------------------------------------------------
HRESULT Compress( const Image& srcImage, DXGI_FORMAT format, DWORD compress, float alphaRef, ScratchImage& image )
{
    if ( IsCompressed(srcImage.format) || !IsCompressed(format) || IsTypeless(format) )
        return E_INVALIDARG;

    // Image size must be a multiple of 4 (degenerate cases for mipmaps are allowed)
    bool degenerate = false;

    size_t width = srcImage.width;
    if ( (width % 4) != 0 )
    {
        if ( width != 1 && width != 2 )
            return E_INVALIDARG;

        degenerate = true;
    }

    size_t height = srcImage.height;
    if ( (height % 4) != 0 )
    {
        if ( height != 1 && height != 2 )
            return E_INVALIDARG;

        degenerate = true;
    }

    // Create compressed image
    HRESULT hr = image.Initialize2D( format, width, height, 1, 1 );
    if ( FAILED(hr) )
        return hr;

    const Image *img = image.GetImage( 0, 0, 0 );
    if ( !img )
    {
        image.Release();
        return E_POINTER;
    }

    // Compress single image
    if ( (compress & TEX_COMPRESS_PARALLEL) && !degenerate )
    {
#ifndef _OPENMP
        return E_NOTIMPL;
#else
        hr = _CompressBC_Parallel( srcImage, *img, _GetBCFlags( compress ), alphaRef );
#endif // _OPENMP
    }
    else
    {
        hr = _CompressBC( srcImage, *img, _GetBCFlags( compress ), alphaRef, degenerate );
    }

    if ( FAILED(hr) )
        image.Release();

    return hr;
}

HRESULT Compress( const Image* srcImages, size_t nimages, const TexMetadata& metadata,
                  DXGI_FORMAT format, DWORD compress, float alphaRef, ScratchImage& cImages )
{
    if ( !srcImages || !nimages )
        return E_INVALIDARG;

    if ( !IsCompressed(format) || IsTypeless(format) )
        return E_INVALIDARG;

    // Image size must be a multiple of 4 (degenerate cases for mipmaps are allowed)
    size_t width = srcImages[0].width;
    if ( (width % 4) != 0 )
    {
        if ( width != 1 && width != 2 )
            return E_INVALIDARG;
    }

    size_t height = srcImages[0].height;
    if ( (height % 4) != 0 )
    {
        if ( height != 1 && height != 2 )
            return E_INVALIDARG;
    }

    cImages.Release();

    TexMetadata mdata2 = metadata;
    mdata2.format = format;
    HRESULT hr = cImages.Initialize( mdata2 );
    if ( FAILED(hr) )
        return hr;

    if ( nimages != cImages.GetImageCount() )
    {
        cImages.Release();
        return E_FAIL;
    }

    const Image* dest = cImages.GetImages();
    if ( !dest  )
    {
        cImages.Release();
        return E_POINTER;
    }

    for( size_t index=0; index < nimages; ++index )
    {
        assert( dest[ index ].format == format );

        const Image& src = srcImages[ index ];

        height = src.height;
        width = src.width;
        if ( width != dest[ index ].width || height != dest[ index ].height )
        {
            cImages.Release();
            return E_FAIL;
        }

        bool degenerate = ((height < 4) || (width < 4)) != 0;

        if ( (compress & TEX_COMPRESS_PARALLEL) && !degenerate)
        {
#ifndef _OPENMP
            return E_NOTIMPL;
#else
            if ( compress & TEX_COMPRESS_PARALLEL )
            {
                hr = _CompressBC_Parallel( src, dest[ index ], _GetBCFlags( compress ), alphaRef );
                if ( FAILED(hr) )
                {
                    cImages.Release();
                    return  hr;
                }
            }
#endif // _OPENMP
        }
        else
        {
            hr = _CompressBC( src, dest[ index ], _GetBCFlags( compress ), alphaRef, degenerate );
            if ( FAILED(hr) )
            {
                cImages.Release();
                return hr;
            }
        }
    }

    return S_OK;
}


//-------------------------------------------------------------------------------------
// Decompression
//-------------------------------------------------------------------------------------
HRESULT Decompress( const Image& cImage, DXGI_FORMAT format, ScratchImage& image )
{
    if ( IsCompressed(format) || IsTypeless(format) )
        return E_INVALIDARG;

    if ( format == DXGI_FORMAT_UNKNOWN )
    {
        // Pick a default decompressed format based on BC input format
        format = _DefaultDecompress( cImage.format );
        if ( format == DXGI_FORMAT_UNKNOWN )
        {
            // Input is not a compressed format
            return E_INVALIDARG;
        }
    }
    else if ( !IsCompressed(cImage.format) || !IsValid(format) )
        return E_INVALIDARG;

    // Create decompressed image
    HRESULT hr = image.Initialize2D( format, cImage.width, cImage.height, 1, 1 );
    if ( FAILED(hr) )
        return hr;

    const Image *img = image.GetImage( 0, 0, 0 );
    if ( !img )
    {
        image.Release();
        return E_POINTER;
    }

    // Decompress single image
    hr = _DecompressBC( cImage, *img );
    if ( FAILED(hr) )
        image.Release();

    return hr;
}

HRESULT Decompress( const Image* cImages, size_t nimages, const TexMetadata& metadata,
                    DXGI_FORMAT format, ScratchImage& images )
{
    if ( !cImages || !nimages )
        return E_INVALIDARG;

    if ( IsCompressed(format) || IsTypeless(format) )
        return E_INVALIDARG;

    if ( format == DXGI_FORMAT_UNKNOWN )
    {
        // Pick a default decompressed format based on BC input format
        format = _DefaultDecompress( cImages[0].format );
        if ( format == DXGI_FORMAT_UNKNOWN )
        {
            // Input is not a compressed format
            return E_FAIL;
        }
    }
    else if ( !IsValid(format) )
        return E_INVALIDARG;

    images.Release();

    TexMetadata mdata2 = metadata;
    mdata2.format = format;
    HRESULT hr = images.Initialize( mdata2 );
    if ( FAILED(hr) )
        return hr;

    if ( nimages != images.GetImageCount() )
    {
        images.Release();
        return E_FAIL;
    }

    const Image* dest = images.GetImages();
    if ( !dest )
    {
        images.Release();
        return E_POINTER;
    }

    for( size_t index=0; index < nimages; ++index )
    {
        assert( dest[ index ].format == format );

        const Image& src = cImages[ index ];
        if ( !IsCompressed( src.format ) )
        {
            images.Release();
            return E_FAIL;
        }

        if ( src.width != dest[ index ].width || src.height != dest[ index ].height )
        {
            images.Release();
            return E_FAIL;
        }

        hr = _DecompressBC( src, dest[ index ] );
        if ( FAILED(hr) )
        {
            images.Release();
            return hr;
        }
    }

    return S_OK;
}

}; // namespace

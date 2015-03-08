//--------------------------------------------------------------------------------------
//CCubeMapProcessor
// Class for filtering and processing cubemaps
//
//
//--------------------------------------------------------------------------------------
// (C) 2005 ATI Research, Inc., All rights reserved.
//--------------------------------------------------------------------------------------

#ifndef CCUBEMAPPROCESSOR_H
#define CCUBEMAPPROCESSOR_H

#include <math.h>
#include <stdio.h>
#include <assert.h>
#include <windows.h>

#include "Types.h"
#include "VectorMacros.h"
#include "CBBoxInt32.h"
#include "CImageSurface.h"

//has routines for saving .rgbe files
#define CG_RGBE_SUPPORT


#ifdef CG_HDR_FILE_SUPPORT
#include "HDRWrite.h"
#endif //CG_HDR_FILE_SUPPORT


#ifndef WCHAR 
#define WCHAR wchar_t
#endif //WCHAR 


//used to index cube faces
#define CP_FACE_X_POS 0
#define CP_FACE_X_NEG 1
#define CP_FACE_Y_POS 2
#define CP_FACE_Y_NEG 3
#define CP_FACE_Z_POS 4
#define CP_FACE_Z_NEG 5


//used to index image edges
// NOTE.. the actual number corresponding to the edge is important
//  do not change these, or the code will break
//
// CP_EDGE_LEFT   is u = 0
// CP_EDGE_RIGHT  is u = width-1
// CP_EDGE_TOP    is v = 0
// CP_EDGE_BOTTOM is v = height-1
#define CP_EDGE_LEFT   0
#define CP_EDGE_RIGHT  1
#define CP_EDGE_TOP    2
#define CP_EDGE_BOTTOM 3

//corners of CUBE map (P or N specifys if it corresponds to the 
//  positive or negative direction each of X, Y, and Z
#define CP_CORNER_NNN  0
#define CP_CORNER_NNP  1
#define CP_CORNER_NPN  2
#define CP_CORNER_NPP  3
#define CP_CORNER_PNN  4
#define CP_CORNER_PNP  5
#define CP_CORNER_PPN  6
#define CP_CORNER_PPP  7

//data types processed by cube map processor
// note that UNORM data types use the full range 
// of the unsigned integer to represent the range [0, 1] inclusive
// the float16 datatype is stored as D3Ds S10E5 representation
#define CP_VAL_UNORM8      0
#define CP_VAL_UNORM8_BGRA 1
#define CP_VAL_UNORM16    10
#define CP_VAL_FLOAT16    20 
#define CP_VAL_FLOAT32    30


//return codes for thread execution
// warning STILL_ACTIVE maps to 259, so the number 259 is reserved in this case
// and should only be used for STILL_ACTIVE
#define CP_THREAD_COMPLETED       0
#define CP_THREAD_TERMINATED     15
#define CP_THREAD_STILL_ACTIVE   STILL_ACTIVE

#define CP_MAX_PROGRESS_STRING 4096

// Type of data used internally by cube map processor
//  just in case for any reason more preecision is needed, 
//  this type can be changed down the road
#define CP_ITYPE float32

// Filter type 
#define CP_FILTER_TYPE_DISC             0
#define CP_FILTER_TYPE_CONE             1
#define CP_FILTER_TYPE_COSINE           2
#define CP_FILTER_TYPE_ANGULAR_GAUSSIAN 3
// SL BEGIN
#define CP_FILTER_TYPE_COSINE_POWER     4
// SL END


// Edge fixup type (how to perform smoothing near edge region)
#define CP_FIXUP_NONE            0
#define CP_FIXUP_PULL_LINEAR     1
#define CP_FIXUP_PULL_HERMITE    2
#define CP_FIXUP_AVERAGE_LINEAR  3
#define CP_FIXUP_AVERAGE_HERMITE 4
// SL BEGIN
#define CP_FIXUP_BENT			 5
#define CP_FIXUP_WARP			 6
#define CP_FIXUP_STRETCH		 7

// Cosine power MipmapChain mode
#define CP_COSINEPOWER_CHAIN_DROP 0
#define CP_COSINEPOWER_CHAIN_MIPMAP 1

// Lighting model
#define CP_LIGHTINGMODEL_PHONG		0
#define CP_LIGHTINGMODEL_PHONG_BRDF	1
#define CP_LIGHTINGMODEL_BLINN		2
#define CP_LIGHTINGMODEL_BLINN_BRDF	3
// SL END


// Max potential cubemap size is limited to 65k (2^16 texels) on a side
#define CP_MAX_MIPLEVELS 16

// SL BEGIN
//maximum number of threads running for cubemap processor is 2
//#define CP_MAX_FILTER_THREADS 2

//initial number of filtering threads for cubemap processor
//#define CP_INITIAL_NUM_FILTER_THREADS 1
// SL END

//current status of cubemap processor
#define CP_STATUS_READY             0
#define CP_STATUS_PROCESSING        1
#define CP_STATUS_FILTER_TERMINATED 2
#define CP_STATUS_FILTER_COMPLETED  3


#define CP_SAFE_DELETE(p)       { if(p) { delete (p);   (p)=NULL; } }
#define CP_SAFE_DELETE_ARRAY(p) { if(p) { delete[] (p); (p)=NULL; } }


//information about cube maps neighboring face after traversing
// across an edge
struct CPCubeMapNeighbor
{
    uint8 m_Face;    //index of neighboring face
    uint8 m_Edge;    //edge in neighboring face that abuts this face
};


//--------------------------------------------------------------------------------------------------
//structure used to store current progress of the filtering
//--------------------------------------------------------------------------------------------------
struct SFilterProgress
{
   //status of current cube map processing
   int32    m_CurrentFace; 
   int32    m_CurrentRow;
   int32    m_CurrentMipLevel; 

   int32    m_StartFace;
   int32    m_EndFace;

   float32  m_FractionCompleted;    //Approximate fraction of work completed for this thread
};

// SL BEGIN

// Struct with all the new option of modified cubemapgen
struct ModifiedCubemapgenOption
{
	float32		  SpecularPower;
	float32		  CosinePowerDropPerMip;
	int32		  NumMipmap;
	int32		  CosinePowerMipmapChainMode;
	bool8		  bExcludeBase;
	bool8		  bIrradianceCubemap;
	int32		  LightingModel;
	int32		  FixupType;
	float32		  GlossScale;
	float32		  GlossBias;
};

struct SThreadFilterFace
{
	class CCubeMapProcessor *m_cmProc;

	CImageSurface*	m_SrcCubeMap; 
	CImageSurface*	m_DstCubeMap;
	float32			m_FilterConeAngle;
	int32			m_FilterType;
	int32			m_FaceIdx; 
	float32		    m_SpecularPower;
	int32		    m_LightingModel;
	float32			m_dotProdThresh;
	int32			m_filterSize;
	int32			m_FixupType;

	HANDLE			m_ThreadHandle;
	DWORD			m_ThreadID;
	SFilterProgress	m_ThreadProgress;
	bool8			m_bUseSolidAngle;
	bool8			m_bThreadInitialized;
};

// SL END

//--------------------------------------------------------------------------------------------------
//structure used to pass filtering parameters for Thread 0
//--------------------------------------------------------------------------------------------------
struct SThreadOptionsThread0
{
   class CCubeMapProcessor *m_cmProc;
   float32 m_BaseFilterAngle;
   float32 m_InitialMipAngle;
   float32 m_MipAnglePerLevelScale;
   int32   m_FilterType;
   // SL BEGIN
   //int32   m_FixupType;
   // SL END
   int32   m_FixupWidth;
   bool8   m_bUseSolidAngle;
   // SL BEGIN
   ModifiedCubemapgenOption m_MCO;
   // SL END
};


//--------------------------------------------------------------------------------------------------
//structure used to pass filtering parameters to the process for Thread 1 (if used)
//--------------------------------------------------------------------------------------------------
struct SThreadOptionsThread1
{
   class CCubeMapProcessor *m_cmProc;
   CImageSurface *m_SrcCubeMap; 
   CImageSurface *m_DstCubeMap;
   float32        m_FilterConeAngle;
   int32          m_FilterType;
   bool8          m_bUseSolidAngle; 
   int32          m_FaceIdxStart; 
   int32          m_FaceIdxEnd; 
   int32          m_ThreadIdx; 
	// SL BEGIN
	ModifiedCubemapgenOption m_MCO;
	// SL END
};


//--------------------------------------------------------------------------------------------------
//Class to filter, perform edge fixup, and build a mip chain for a cubemap
//--------------------------------------------------------------------------------------------------
class CCubeMapProcessor
{
public:

   //cubemap processor status
   int32             m_Status;

   //information about threads actively processing the cubemap
   int32             m_NumFilterThreads;
   // SL BEGIN
   // Suppose only one CCubeMapProcessor at a time
   static SThreadFilterFace* sg_ThreadFilterFace;
   HANDLE DumbThreadHandle; // thread id of the main thread when in multithread mode
   // SL END
   WCHAR             m_ProgressString[CP_MAX_PROGRESS_STRING];

   //filtering parameters last used for filtering
   float32           m_BaseFilterAngle;
   float32           m_InitialMipAngle;
   float32           m_MipAnglePerLevelScale;

   int32 m_InputSize;                     //input cubemap size  (e.g. face width and height of topmost mip level)
   int32 m_OutputSize;                    //output cubemap size (e.g. face width and height of topmost mip level)
   int32 m_NumMipLevels;                  //number of output mip levels
   int32 m_NumChannels;                   //number of channels in cube map processor

   CP_ITYPE *m_FilterLUT;                 //filter weight lookup table (scale dot product 0-1 range to index into it)
   int32   m_NumFilterLUTEntries;         //number of filter lookup table entries
   
   CImageSurface m_NormCubeMap[6];        //normalizer cube map and solid angle lookup table

   CImageSurface m_InputSurface[6];       //input faces for topmost mip level

   CImageSurface m_OutputSurface[CP_MAX_MIPLEVELS][6];   //output faces for all mip levels

// SL BEGIN
public:
// SL END
	//==========================================================================================================
   //BuildNormalizerCubemap(int32 a_Size, CImageSurface *a_Surface );
   // Builds a normalizer cubemap of size a_Size.  This routine deallocates the CImageSurfaces passed 
   // into the the function and reallocates them with the correct size and 3 channels to store the 
   // normalized vector for each texel.
   //
   // a_Size         [in]  size of normalizer cubemap
   // a_Surface      [out]  Pointer to array of 6 CImageSurfaces where normalizer cube faces will be stored
   //                   
   //==========================================================================================================
   // SL BEGIN
   void BuildNormalizerCubemap(int32 a_Size, CImageSurface *a_Surface, int32 a_FixupType);
   // SL END

   //==========================================================================================================
   //void BuildNormalizerSolidAngleCubemap(int32 a_Size, CImageSurface *a_Surface );
   // Builds a normalizer|solid angle cubemap of size a_Size. This routine deallocates the CImageSurfaces 
   // passed into the the function and reallocates them with the correct size and 4 channels to store the 
   // normalized vector, and solid angle for each texel.
   //
   // a_Size         [in]
   // a_Surface      [out]  Pointer to array of 6 CImageSurfaces where normalizer cube faces will be stored
   //
   //==========================================================================================================
   // SL BEGIN
   void BuildNormalizerSolidAngleCubemap(int32 a_Size, CImageSurface *a_Surface, int32 a_FixupType);
   // SL END

   //==========================================================================================================
   //Clears filter extent bounding boxes for each face of the cubemap
   //
   // a_FilterExtents [in]  Array of 6 bounding boxes (corresponding to the 6 cubemap faces) to clear
   //==========================================================================================================
   void ClearFilterExtents(CBBoxInt32 *a_FilterExtents);
   
   //==========================================================================================================
   //void DetermineFilterExtents(float32 *a_CenterTapDir, int32 a_SrcSize, int32 a_BBoxSize, 
   //    CBBoxInt32 *a_FilterExtents);
   //
   //Determines bounding boxes for each cube face for a single kernels angular extent 
   // a_CenterTapDir  [in]  Vector of 3 float32s specifying the center tap direction
   // a_SrcSize       [in]  Source cubemap size (for the miplevel used as input to the filtering)
   // a_BBoxSize      [in]  Maximum length in texels of the bbox extent derived from the filtering 
   //                       cone angle
   // a_FilterExtents [out] Array of 6 bounding boxes (corresponding to the 6 cubemap faces) to clear
   //==========================================================================================================
   void DetermineFilterExtents(float32 *a_CenterTapDir, int32 a_SrcSize, int32 a_BBoxSize, CBBoxInt32 *a_FilterExtents);
   
   //==========================================================================================================
   //void ProcessFilterExtents(float32 *a_CenterTapDir, float32 a_DotProdThresh, CBBoxInt32 *a_FilterExtents, 
   //   CImageSurface *a_NormCubeMap, CImageSurface *a_SrcCubeMap, CP_ITYPE *a_DstVal, uint32 a_FilterType, 
   //   bool8 a_bUseSolidAngle );
   //
   //Processes all the texels within the bounding boxes in order to accumulate all the weighted taps to 
   // compute a single fitered texel value.
   //
   //a_CenterTapDir     [in]  Center tap directions
   //a_DotProdThresh    [in]  Threshhold on dot product between center tap and 
   //a_FilterExtents    [in]  array of 6 bounding boxes describing rough filter extents for each face
   //a_NormCubeMap      [in]  normalizer|solid angle cubemap
   //a_SrcCubeMap       [in]  array of 6 faces comprising the miplevel of the source cubemap the filter is 
   //                         generated from
   //a_DstVal           [out] resulting filtered texel color
   //a_FilterType       [in]  filter type: Choose one of the following options: CP_FILTER_TYPE_DISC, 
   //                         CP_FILTER_TYPE_CONE, CP_FILTER_TYPE_COSINE, CP_FILTER_TYPE_ANGULAR_GAUSSIAN
   //a_bUseSolidAngle   [in]  Set this to true in order to incorporate the solid angle subtended 
   //                         each texel in the filter kernel in the filtering.
   //
   //==========================================================================================================
   void ProcessFilterExtents(float32 *a_CenterTapDir, float32 a_DotProdThresh, CBBoxInt32 *a_FilterExtents, 
      CImageSurface *a_NormCubeMap, CImageSurface *a_SrcCubeMap, CP_ITYPE *a_DstVal, uint32 a_FilterType, 
      bool8 a_bUseSolidAngle
	  // SL BEGIN
	  , float32 a_SpecularPower
	  ,int32 a_LightingModel
	  // SL END
	  );

   //==========================================================================================================
   //void FixupCubeEdges(CImageSurface *a_CubeMap, int32 a_FixupType, int32 a_FixupWidth);
   //
   //Apply edge fixup to a cubemap mip level.
   //
   //a_CubeMap       [in/out] Array of 6 images comprising cubemap miplevel to apply edge fixup to.
   //a_FixupType     [in]     Specifies the technique used for edge fixup.  Choose one of the following, 
   //                         CP_FIXUP_NONE, CP_FIXUP_PULL_LINEAR, CP_FIXUP_PULL_HERMITE, CP_FIXUP_AVERAGE_LINEAR, 
   //                         CP_FIXUP_AVERAGE_HERMITE, CP_FIXUP_BENT, CP_FIXUP_WARP, CP_FIXUP_STRETCH
   //a_FixupWidth    [in]     Fixup width in texels
   //
   //==========================================================================================================
   void FixupCubeEdges(CImageSurface *a_CubeMap, int32 a_FixupType, int32 a_FixupWidth);

   //==========================================================================================================
   //void BuildAngleWeightLUT(int32 a_NumFilterLUTEntries, int32 a_FilterType, float32 a_FilterAngle);
   //
   // Builds filter weight lookup table in order to quickly evaluate the weight of a particular texel
   //  for the Cone and Angular Gaussian fiter types.  This lookup table is quickly indexed using the 
   //  same dot product between the center tap and current texel that is used to determine whether a  
   //  texel is inside or outside the filtering kernel.
   //
   //a_NumFilterLUTEntries [in]     Number of entries in filter weight lookup table
   //a_FilterType          [in]     Filter type 
   //a_FilterAngle         [in]     Filtering half cone angle
   //==========================================================================================================
   void BuildAngleWeightLUT(int32 a_NumFilterLUTEntries, int32 a_FilterType, float32 a_FilterAngle);

   //==========================================================================================================
   //void PrecomputeFilterLookupTables(uint32 a_FilterType, int32 a_SrcCubeMapWidth, float32 a_FilterConeAngle);
   //
   // Builds the following lookup tables prior to filtering:
   //  -normalizer cube map
   //  -filter weight lookup table
   //
   //a_FilterType          [in]     Filter type 
   //a_SrcCubeMapWidth     [in]     source cubemap size
   //a_FilterConeAngle     [in]     Filtering half cone angle
   //==========================================================================================================
   // SL BEGIN
   void PrecomputeFilterLookupTables(uint32 a_FilterType, int32 a_SrcCubeMapWidth, float32 a_FilterConeAngle, int32 a_FixupType);
   // SL END

   //==========================================================================================================
   //void EstimateFilterThreadProgress(SFilterProgress *a_FilterProgress);
   //
   // Estimates percentage complete for a filtering thread for the current tap that is being filtered
   //
   //a_FilterProgress   [in/out]  Information about the filtereing thread's current position, and range of faces
   //                             that it will process.
   //==========================================================================================================
   void EstimateFilterThreadProgress(SFilterProgress *a_FilterProgress);

public:
   //==========================================================================================================
   //note that these 2 functions are only public so that they can be called from within the global scope 
   // from the thread starting point functions.  These should not be called by any other functions external 
   // to the class.
   //==========================================================================================================
   void FilterCubeMapMipChain(float32 a_BaseFilterAngle, float32 a_InitialMipAngle, float32 a_MipAnglePerLevelScale, 
	   	 // SL BEGIN
		int32 a_FilterType, /* int32 a_FixupType, */ int32 a_FixupWidth, bool8 a_bUseSolidAngle
		, const ModifiedCubemapgenOption& m_MCO
		// SL END
	  );
   void FilterCubeSurfaces(CImageSurface *a_SrcCubeMap, CImageSurface *a_DstCubeMap, float32 a_FilterConeAngle, 
      int32 a_FilterType, bool8 a_bUseSolidAngle, int32 a_FaceIdxStart, int32 a_FaceIdxEnd, int32 aThreadIdx
	  	// SL BEGIN
		, float32 a_SpecularPower, int32 a_LightingModel, int32 a_FixupType
		// SL END
	  );        

   // SL BEGIN
   // To process an irradiance cubemap
   void SHFilterCubeMap(bool8 a_bUseSolidAngleWeighting, int32 a_FixupType);

   void FilterCubeMapMipChainMultithread(float32 a_BaseFilterAngle, float32 a_InitialMipAngle, float32 a_MipAnglePerLevelScale, 
		int32 a_FilterType, int32 a_FixupWidth, bool8 a_bUseSolidAngle, const ModifiedCubemapgenOption& m_MCO);

   void FilterCubeSurfacesMultithread(CImageSurface *a_SrcCubeMap, CImageSurface *a_DstCubeMap, 
		float32 a_FilterConeAngle, int32 a_FilterType, bool8 a_bUseSolidAngle, float32 a_SpecularPower, uint32 a_MipIndex, int32 a_LightingModel, int32 a_FixupType);
   // SL END

public:
   CCubeMapProcessor(void);
   ~CCubeMapProcessor();

   //==========================================================================================================
   // void Init(int32 a_InputSize, int32 a_OutputSize, int32 a_NumMipLevels, int32 a_NumChannels);
   //
   // Initializes cube map processor class
   //
   // a_InputSize    [in]     Size of the input cubemap
   // a_OutputSize   [in]     Size of the output cubemap
   // a_NumMipLevels [in]     Number of miplevels in the output cubemap
   // a_NumChannels  [in]     Number of color channels (internally) in the input and output cubemap
   //==========================================================================================================
   void Init(int32 a_InputSize, int32 a_OutputSize, int32 a_NumMipLevels, int32 a_NumChannels);


   //==========================================================================================================
   // void GetInputFaceData(int32 a_FaceIdx, int32 a_DstType, int32 a_DstNumChannels, int32 a_DstPitch, 
   //   void *a_DstDataPtr, float32 a_Scale, float32 a_Gamma);
   //
   // Copies image data from the input cube map into a destination image.  These routine describe the output
   // image layout using a pitch and a pointer so that the image can be copied from a subrect of a locked 
   // D3D surface easily.  Note that when reading out the image data, the intensity scale is applied first,
   // and then degamma. 
   //
   //  a_FaceIdx        [in]     Index (0-5) of the input cubemap cube face to read the image data from.
   //  a_DstType        [in]     Data type for the image data being copied out the input cube map.
   //                            choose one of the following: CP_VAL_UNORM8, CP_VAL_UNORM8_BGRA, CP_VAL_UNORM16    
   //                            CP_VAL_FLOAT16, CP_VAL_FLOAT32.
   //  a_DstNumChannels [in]     Number of channels in the destination image.
   //  a_DstPitch       [in]     Pitch in bytes of the destination image.
   //  a_DstDataPtr     [in]     Pointer to the top-left pixel in the destination image.
   //  a_Scale          [in]     Scale factor to apply to intensities.
   //  a_Gamma          [in]     Degamma to apply to intensities.
   //
   //==========================================================================================================
   void GetInputFaceData(int32 a_FaceIdx, int32 a_DstType, int32 a_DstNumChannels, int32 a_DstPitch, 
      void *a_DstDataPtr, float32 a_Scale, float32 a_Gamma );


   //==========================================================================================================
   // void SetInputFaceData(int32 a_FaceIdx, int32 a_SrcType, int32 a_SrcNumChannels, int32 a_SrcPitch, 
   //    void *a_SrcDataPtr, float32 a_MaxClamp, float32 a_Scale, float32 a_Gamma );
   //
   // Copies image data from a source image into one of the faces in the input cubemap in the cubemap.  
   // processor.  These routines describe the output image layout using a pitch and a pointer so that the image 
   // can be copied from a subrect of a locked D3D surface easily.  Note that the clamping is applied first, 
   // followed by the scale and then gamma.
   //
   //  a_FaceIdx        [in]     Index (0-5) of the input cubemap cube face to write the image data into
   //  a_SrcType        [in]     Data type for the image data being copied into the cube map processor.
   //                            choose one of the following: CP_VAL_UNORM8, CP_VAL_UNORM8_BGRA, CP_VAL_UNORM16    
   //                            CP_VAL_FLOAT16, CP_VAL_FLOAT32.
   //  a_SrcNumChannels [in]     Number of channels in the source image.
   //  a_SrcPitch       [in]     Pitch in bytes of the source image.
   //  a_SrcDataPtr     [in]     Pointer to the top-left pixel in the source image.
   //  a_MaxClamp       [in]     Max value to clamp the input intensity values to.
   //  a_Degamma        [in]     Degamma to apply to input intensities.
   //  a_Scale          [in]     Scale factor to apply to input intensities.
   //
   //==========================================================================================================
   void SetInputFaceData(int32 a_FaceIdx, int32 a_SrcType, int32 a_SrcNumChannels, int32 a_SrcPitch, 
      void *a_SrcDataPtr, float32 a_MaxClamp, float32 a_Degamma, float32 a_Scale );


   //==========================================================================================================
   // void GetOutputFaceData(int32 a_FaceIdx, int32 a_Level, int32 a_DstType, int32 a_DstNumChannels, int32 a_DstPitch, 
   //    void *a_DstDataPtr, float32 a_Scale, float32 a_Gamma );   
   //
   //  a_FaceIdx        [in]     Index (0-5) of the output cubemap cube face to read the image data from.
   //  a_Level          [in]     Miplevel of the output cubemap to read from
   //  a_DstType        [in]     Data type for the image data being copied out the input cube map.
   //                            choose one of the following: CP_VAL_UNORM8, CP_VAL_UNORM8_BGRA, CP_VAL_UNORM16    
   //                            CP_VAL_FLOAT16, CP_VAL_FLOAT32
   //  a_DstNumChannels [in]     Number of channels in the destination image.
   //  a_DstPitch       [in]     Pitch in bytes of the destination image.
   //  a_DstDataPtr     [in]     Pointer to the top-left pixel in the destination image.
   //  a_Scale          [in]     Scale factor to apply to intensities.
   //  a_Gamma          [in]     Degamma to apply to intensities.
   //
   //==========================================================================================================
   void GetOutputFaceData(int32 a_FaceIdx, int32 a_Level, int32 a_DstType, int32 a_DstNumChannels, int32 a_DstPitch, 
      void *a_DstDataPtr, float32 a_Scale, float32 a_Gamma );   


   //==========================================================================================================
   //void InitiateFiltering(float32 a_BaseFilterAngle, float32 a_InitialMipAngle, float32 a_MipAnglePerLevelScale, 
   //   int32 a_FilterType, int32 a_FixupType, int32 a_FixupWidth, bool8 a_bUseSolidAngle );
   //
   // Starts filtering the cubemap.  
   // If the number of filter threads is zero, the function does not return until the filtering is complete
   // If the number of filter threads is non-zero, a filtering thread (or multiple threads) are started and 
   //  the function returns immediately, with the threads running in the background.
   //
   // The cube map filtereing is specified using a number of parameters:
   // Filtering per miplevel is specified using 2D cone angle (in degrees) that 
   //  indicates the region of the hemisphere to filter over for each tap. 
   //                
   // Note that the top mip level is also a filtered version of the original input images 
   //  as well in order to create mip chains for diffuse environment illumination.
   //  The cone angle for the top level is specified by a_BaseAngle.  This can be used to
   //  generate mipchains used to store the results of preintegration across the hemisphere.
   //
   // The angle for the subsequent levels of the mip chain are specified by their parents 
   //  filtering angle and a per-level scale and bias
   //   newAngle = oldAngle * a_MipAnglePerLevelScale;
   //  
   //  a_BaseFilterAngle          [in] Base filter angle
   //  a_InitialMipAngle          [in] Mip angle used to generate the next level of the mip chain from the base level 
   //  a_MipAnglePerLevelScale    [in] Scale factor to iteratively apply to the filtering angle to filter subsequent 
   //                                  mip-levels.
   //  a_FilterType               [in] Specifies the filtering type for angular extent filtering. Choose one of the 
   //                                  following options: CP_FILTER_TYPE_DISC, CP_FILTER_TYPE_CONE, 
   //                                  CP_FILTER_TYPE_COSINE, CP_FILTER_TYPE_ANGULAR_GAUSSIAN
   //  a_FixupType                [in] Specifies the technique used for edge fixup.  Choose one of the following, 
   //                                  CP_FIXUP_NONE, CP_FIXUP_PULL_LINEAR, CP_FIXUP_PULL_HERMITE, 
   //                                  CP_FIXUP_AVERAGE_LINEAR, CP_FIXUP_AVERAGE_HERMITE, CP_FIXUP_BENT, CP_FIXUP_WARP, CP_FIXUP_STRETCH
   //  a_FixupWidth               [in] Width in texels of the fixup region.
   //  a_bUseSolidAngle           [in] Set this to true in order to incorporate the solid angle subtended 
   //                                  each texel in the filter kernel in the filtering.
   //==========================================================================================================
   void InitiateFiltering(float32 a_BaseFilterAngle, float32 a_InitialMipAngle, float32 a_MipAnglePerLevelScale, 
      int32 a_FilterType, int32 a_FixupType, int32 a_FixupWidth, bool8 a_bUseSolidAngle
	  // SL BEGIN
	  , bool8 a_bUseMultithread, float32 a_SpecularPower, float32 a_CosinePowerDropPerMip, int32 a_NumMipmap, int32 a_CosinePowerMipmapChainMode
	  ,	bool8 a_bExcludeBase, bool8 a_bIrradianceCubemap, int32 a_LightingModel, float32 a_GlossScale, float32 a_GlossBias
	  // SL END
	  );

   //==========================================================================================================
   // void WriteMipLevelIntoAlpha(void)
   //
   //  Encodes the miplevel in the alpha channel of the output cubemap.
   //  The miplevel is encoded as (miplevel * 16.0f / 255.0f) so that the miplevel has an exact encoding in an 
   //  8-bit or 16-bit UNORM representation.
   //
   //==========================================================================================================
   void WriteMipLevelIntoAlpha(void);


   //==========================================================================================================
   // Horizontally flips all the faces in the input cubemap
   //   
   //==========================================================================================================
   void FlipInputCubemapFaces(void);

   //==========================================================================================================
   // Horizontally flips all the faces in the output cubemap
   //   
   //==========================================================================================================
   void FlipOutputCubemapFaces(void);


   //==========================================================================================================
   // Allows for in-place color channel swapping of the input cubemap.  This routine can be useful for 
   // converting RGBA format data to BGRA format data.
   //
   // a_Channel0Src           [in] Index of the color channel used as the source for the new channel 0
   // a_Channel1Src           [in] Index of the color channel used as the source for the new channel 1
   // a_Channel2Src           [in] Index of the color channel used as the source for the new channel 0
   // a_Channel3Src           [in] Index of the color channel used as the source for the new channel 1
   //
   //==========================================================================================================
   void ChannelSwapInputFaceData(int32 a_Channel0Src, int32 a_Channel1Src, int32 a_Channel2Src, int32 a_Channel3Src );


   //==========================================================================================================
   // Allows for in-place color channel swapping of the output cubemap.  This routine can be useful for 
   // converting RGBA format data to BGRA format data.
   //
   // a_Channel0Src           [in] Index of the color channel used as the source for the new channel 0
   // a_Channel1Src           [in] Index of the color channel used as the source for the new channel 1
   // a_Channel2Src           [in] Index of the color channel used as the source for the new channel 0
   // a_Channel3Src           [in] Index of the color channel used as the source for the new channel 1
   //==========================================================================================================
   void ChannelSwapOutputFaceData(int32 a_Channel0Src, int32 a_Channel1Src, int32 a_Channel2Src, int32 a_Channel3Src );


   //==========================================================================================================
   // Resets the current cubemap processor, and deallocates the input and output cubemaps.
   //
   // This function is automatically called by destructor.
   //==========================================================================================================
   void Clear(void);


   //==========================================================================================================
   // Terminates any active filtering threads.  This stops the filtering of the current cubemap.
   //
   //==========================================================================================================
   void TerminateActiveThreads(void);


   //==========================================================================================================
   // Gets the current filtering progress string 
   //
   //==========================================================================================================
   WCHAR *GetFilterProgressString(void);


   //==========================================================================================================
   // Checks to see if either of the filtering threads is active 
   //
   //==========================================================================================================
   bool8 IsFilterThreadActive(uint32 a_ThreadIdx);

   //==========================================================================================================
   // Gets the current status of the cubemap processing threads.  The possible return values and their 
   // associated meanings are:
   //
   // CP_STATUS_READY:               The cubemap processor is currently ready to change settings, and to load a 
   //                                new input cubemap.                                 
   // CP_STATUS_PROCESSING:          The cubemap processor is currently filtering a cubemap
   // CP_STATUS_FILTER_TERMINATED:   The cubemap processor was terminated before the filtering was completed.                                 
   // CP_STATUS_FILTER_COMPLETED:    The cubemap processor fully completed filtering the cubemap.
   //
   //==========================================================================================================
   int32 GetStatus(void);


   //==========================================================================================================   
   // This signifies to the cubemap processor that you have acknowledged a 
   //  CP_STATUS_FILTER_TERMINATED or CP_STATUS_FILTER_COMPLETED status code, and would like to 
   //  reset the cubemap processor to CP_STATUS_READY. 
   //==========================================================================================================
   void RefreshStatus(void);

};


#endif //CCUBEMAPFILTER_H

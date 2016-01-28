#ifndef DATA_STRUCT_DEFS
#define DATA_STRUCT_DEFS

__declspec(align(4)) struct StylusDataFrame 
{
	BINHDR hrd;
	FLOAT xPos;
	FLOAT yPos;
	FLOAT zPos;
	ULONG  lStylus;
};

__declspec(align(4)) struct MarkerDataFrame
{
	BINHDR hrd;
	FLOAT xPos;
	FLOAT yPos;
	FLOAT zPos;
	FLOAT azimuth;
	FLOAT elevation;
	FLOAT roll;
};


#endif // DATA_STRUCT_DEFS

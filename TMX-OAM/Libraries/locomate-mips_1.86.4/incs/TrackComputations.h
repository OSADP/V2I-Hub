#ifndef TRACCOMPUTATIONS_H
#define TRACCOMPUTATIONS_H

/*
The following data structures and functions were not exposed earlier.
They were accessed via the test interface CrossTrackError_TestInterface.h.

They are now being exposed as the functionality has to be used for processing 
GGANavData
*/

struct EllipsoidalCoords {
        double lat;
        double lon;
        double elev; 
        };

#ifndef ECEFCOORDS
#define ECEFCOORDS
struct ECEFCoords{
        double X; 
        double Y;
        double Z;
		//All units are meters
        };
#endif

#define CARTESIANCOORDS struct ECEFCoords

//struct Vector{
//        double X; //degrees
//        double Y; //degrees
//        double Z; //meters
//        };
//
#define ELLIPSOID_MAJOR_AXIS 6378137.0
#define ELLIPSOID_MINOR_AXIS 6356752.3142
//
//
#ifndef PI
#define PI (4 * atan(1.0))  
#endif

#ifndef TRUE
#define TRUE 1
#endif

#ifndef FALSE
#define FALSE 0
#endif       
 
#define R1 pow((long double)ELLIPSOID_MAJOR_AXIS, 2)
#define R2 pow((long double)ELLIPSOID_MINOR_AXIS, 2)
#define R2divR1  ((long double)R2/(long double)R1)
#define SqrOfTheEccentricity (long double)(1 - R2divR1)
/*
This function returns the heading with respect to WE axis.
East is 0. N is PI/2, S is -PI/2. W is -PI
*/
double fnComputeAngleWRT_WE(struct EllipsoidalCoords *ptrToPoint_A, \
											struct EllipsoidalCoords *ptrToPoint_B);

/*
ThE routine fnComputeOffsetPointUsingHeadingLatLong generates the lat and long of a point that is 
offset from a reference point on a vehicle.

THE ORIGIN OF THE REFERENCE SYSTEM IS THE GPS ANTENNA THAT IS USED FOR GETTING THE GPS DATA STREAM.

The X-Axis of the reference system for specifying the offset, is in the horizontal plane, passes through the reference point with positive being towards the front of the vehicle. The Y-Axis is also in the horizontal plane, passes through the reference point and has the left of the vehicle as the +ve direction. The Z-Axis is Vertical with up direction being +ve

The track of the vehicle is assumed to be along the X-Axis. 

The assumption is NOT valid:
		In a skid. (The yaw angle must be estimated.)
		When reversing. (The vehicle is moving along the -ve X-Axis

The above two conditions will require the code below to be wrappered.

The computation uses lat long and does not transform to ECEF cordinates
*/
struct EllipsoidalCoords *fnComputeOffsetPointUsingHeadingLatLong( \
									struct EllipsoidalCoords *ptrToPredecessorPoint, \
									struct EllipsoidalCoords *ptrToPoint, \
									CARTESIANCOORDS *ptrToOffsetCoords,unsigned char TrackIsStationary, \
									unsigned char OffsetWRTPredecessor,long double *ptrToPrevHeading);

struct EllipsoidalCoords *fnDel_X_Del_YToLatLong(struct EllipsoidalCoords *ptrToRefPoint, \
													CARTESIANCOORDS *ptrToDel_X_Del_Y);


#endif

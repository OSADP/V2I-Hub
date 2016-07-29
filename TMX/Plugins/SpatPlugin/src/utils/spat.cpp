/*************************************************************************************
*
*  spat.cpp   : SPAT Message Procedures
*
*  purpose    : Provide procedures for Initialization, Encoding, and Decoding
*               SPAT message structure
*
*************************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include "spat.h"

/* SPAT Structures */
struct spat_Maneuver
    {
    unsigned short Maneuver[spat_maxmovements];
    spat_data *data;
    };
    
/* Functions */

/*************************************************************************************
*  Name       : spat_clear 
*  Purpose    : Clear the contents of the SPAT structure
*  Parameters : message  - Pointer to the SPAT message structure
*************************************************************************************/
void spat_clear(spat *message)
    {
	int l, m;

/*  Clear all Data from the SPAT Message Structure */
	(*message).intersectionid = 0;
	(*message).intersectionstatus = 0;
	(*message).timestampseconds = 0;
	(*message).timestamptenths = 0;
	for(m=0; m<spat_maxmovements; m++)
	    {

/*      Clear all Data from the Movement Structure */
        for(l=0; l<spat_maxlanes; l++) (*message).movement[m].lane[l] = 0;
        (*message).movement[m].type = 0;
        (*message).movement[m].data.state = 0;
        (*message).movement[m].data.mintime = 0;
        (*message).movement[m].data.maxtime = 0;
        (*message).movement[m].data.yellowstate = 0;
        (*message).movement[m].data.yellowtime = 0;
        (*message).movement[m].data.pedestrian = unavailable;
        (*message).movement[m].data.count = 0;
        }
    }
    
/*************************************************************************************
*  Name       : spat_equals 
*  Purpose    : Determine the data elements between two SPAT movements are equal.
*  Parameters : a  - Pointer to the first movement data structure 
*               b  - Pointer to the second movement data structure
*************************************************************************************/
int spat_equals(spat_data *a, spat_data *b)
    {
    if ((*a).state       != (*b).state)       return 0;
    if ((*a).mintime     != (*b).mintime)     return 0;
    if ((*a).maxtime     != (*b).maxtime)     return 0;
    if ((*a).yellowstate != (*b).yellowstate) return 0;
    if ((*a).yellowtime  != (*b).yellowtime)  return 0;
    if ((*a).pedestrian  != (*b).pedestrian)  return 0;
    if ((*a).count       != (*b).count)       return 0;
    return 1;
    }
           
/*************************************************************************************
*  Name       : spat_copy 
*  Purpose    : Copy the data elements from one SPAT movement to another
*  Parameters : a  - Pointer to the first movement data structure
*               b  - Pointer to the second movement data structure
*************************************************************************************/
int spat_copy(spat_data *a, spat_data *b)
    {
    (*a).state       = (*b).state;
    (*a).mintime     = (*b).mintime;
    (*a).maxtime     = (*b).maxtime;
    (*a).yellowstate = (*b).yellowstate;
    (*a).yellowtime  = (*b).yellowtime;
    (*a).pedestrian  = (*b).pedestrian;
    (*a).count       = (*b).count;
    return 1;
    }

/*************************************************************************************
*  Name       : spat_clip 
*  Purpose    : Clip the Time Remaining Values to a Maximum of 120.0 seconds or
*               return a 1202 for an undefined amount of time.
*  Parameters : value  - Value of the Time Remaining
*************************************************************************************/
unsigned int spat_clip(unsigned int value)
    {
	if (value == 0xffff) return 1202;
	if (value > 1200) return 1201;
	return value;
    }

/*************************************************************************************
*  Name       : spat_compile
*  Purpose    : Compile the SPAT message for encoding
*  Parameters : message  - Pointer to the SPAT message structure
*************************************************************************************/
int spat_compile(spat *message, spat_Maneuver *Maneuver)
    {
    int i, j, l, m, n, count;
    int used[spat_maxmovements];
    int index[spat_maxmovements][spat_maxmovements];
    spat_movement *pm;
    
/*  Initialize Variables */
    for (i=0; i<spat_maxmovements; i++) 
        {
        used[i]= 0;
        for (j=0; j<spat_maxmovements; j++) {index[i][j] = -1; Maneuver[i].Maneuver[j] = 0;}        
        }

/*  Compile the Movement Matrix */
    i = 0;
    for (m=0; m<spat_maxmovements; m++) 
        {
/*        if ((*message).movement[m].type == 0) {break;}*/
        if ((*message).movement[m].type > 16) {break;}
        if (!used[m])
            {
            j = 0;
            for (n=m; n<spat_maxmovements; n++) 
                {
                if ((*message).movement[n].type == 0) {break;}
                if (!used[n])
                    {
                    if (spat_equals(&(*message).movement[m].data, &(*message).movement[n].data))
                        {
                        index[i][j++] = n;
                        used[n] = 1;
                        }
                    }
                }
			i++;   
			}   
        }
    count = i; 
    
/*  Compile the Movement Maneuver List */
    for (i=0; i<count; i++) 
        {
        (*Maneuver).data = &(*message).movement[index[i][0]].data;
        j=0;
        while (index[i][j] != -1)
            {
            l=0;
            pm = &(*message).movement[index[i][j]];
            while ((*pm).lane[l] != 0)
                {
                for (m=0; m<spat_maxmovements; m++)
                    {
                    if ((*Maneuver).Maneuver[m] == 0) {(*Maneuver).Maneuver[m] = ((*pm).type << 8) | (*pm).lane[l]; break;}
                    else {if (((*Maneuver).Maneuver[m] & 0xff) == (*pm).lane[l]) {(*Maneuver).Maneuver[m] |= ((*pm).type << 8) ; break;}}
                    }
                l++;
                }  
            j++;         
            }
        Maneuver++;
        }
        
/*  Return the Size of the Maneuvers List */
    return count;    
    }
   
/*************************************************************************************
*  Name       : spat_decompile
*  Purpose    : Decompile the SPAT message
*  Parameters : message  - Pointer to the SPAT message structure
*************************************************************************************/
int spat_decompile(int i, unsigned char *pblob, spat_movement *pm)
    {
    int l, j;
    int movements;
    int isize; 
    int icount;
       
/*  Get this Size */
    isize = pblob[i++];
    icount = 0;
       
/*  Combine all the Movement Codes */        
    movements = 0;
    for (j=0; j<isize; j++) {movements |= pblob[i+j]; j++;}

/*  Decompile the Straight Ahead Movement */
    if (movements & straight)
        {
        (*pm).type = straight;
        l=0;
        for (j=0; j<isize; j++) {if (pblob[(i)+(j++)] & straight) {(*pm).lane[l++] = pblob[i+j];}}
        pm++;
        icount++;
        }
    
/*  Decompile the Left Turn Movement */
    if (movements & leftturn)
        {
        (*pm).type = leftturn;
        l=0;
        for (j=0; j<isize; j++) {if (pblob[(i)+(j++)] & leftturn) {(*pm).lane[l++] = pblob[i+j];}}
        pm++;
        icount++;
        }
        
/*  Decompile the Right Turn Movement */
    if (movements & rightturn)
        {
        (*pm).type = rightturn;
        l=0;
        for (j=0; j<isize; j++) {if (pblob[(i)+(j++)] & rightturn) {(*pm).lane[l++] = pblob[i+j];}}
        pm++;
        icount++;
        }
        
/*  Decompile the U Turn Movement */
    if (movements & uturn)
        {
        (*pm).type = uturn;
        l=0;
        for (j=0; j<isize; j++) {if (pblob[(i)+(j++)] & uturn) {(*pm).lane[l++] = pblob[i+j];}}
        pm++;
        icount++;
        }
        
/*  Return the Number of Movements */
    return icount;    
    }
     
/* SPAT Procedures */

/*************************************************************************************
*  Name       : spat_initialize 
*  Purpose    : Initialize the SPAT structure
*  Parameters : message  - Pointer to the SPAT message structure
*************************************************************************************/
int spat_initialize(spat *message)
    {
    spat_clear(message);
    (*message).payload.pblob = NULL;
    (*message).payload.size = 0;
    (*message).payload.messageid = 0;
    (*message).payload.version = 0;
	return 1;
    }   
    
/*************************************************************************************
*  Name       : spat_encode 
*  Purpose    : Encode the SPAT message into the blob payload
*  Parameters : message          - Pointer to the SPAT message structure
*************************************************************************************/
int spat_encode(spat *message, unsigned char content_version)
    {
    int i, j, m, n, s, count;
    unsigned char *pblob;
    unsigned short crc;
    spat_Maneuver Maneuver[spat_maxmovements];

/*  Free any Previously Allocated Memory */
    blob_free(&(*message).payload);
    
/*  Create the SPAT Blob */
    pblob = (unsigned char*) malloc(1024);
    if (pblob == NULL) 
        { 
        return 0;
        }  
    for (i=0; i<1024; i++) pblob[i] = 0;

    i = 0;
/*  Encode the Message Header */

    pblob[i++] = 0x8d;
    pblob[i++] = content_version;
    s = i++;
    i++;
    
/*  Encode the Intersection ID */
    pblob[i++] = 1;
    pblob[i++] = 4;    
    for (j=0; j<4; j++) pblob[i++] = (unsigned char)(((*message).intersectionid & (0xff000000 >> (8*j))) >> (8*(3-j)));

/*  Encode the Intersection Status */
    pblob[i++] = 2;
    pblob[i++] = 1;
    pblob[i++] = (*message).intersectionstatus;

/*  Encode the Timestamp */
    pblob[i++] = 3;
    pblob[i++] = 5;
    for (j=0; j<4; j++) pblob[i++] = (unsigned char)(((*message).timestampseconds & (0xff000000 >> (8*j))) >> (8*(3-j)));
    pblob[i++] = (*message).timestamptenths;

/*  Compile the SPAT Message */
    count = spat_compile(message, &Maneuver[0]);
    
/*  Encode Each Movement */
    for(m=0; m<count; m++)
        {
        pblob[i++] = 4;

/*      Encode the Lane Set */
        pblob[i++] = 5;
        pblob[i++] = 0;
        n=0;
        while (Maneuver[m].Maneuver[n] !=0) {for (j=0; j<2; j++) pblob[i++] = (Maneuver[m].Maneuver[n] & (0xff00 >> (8*j))) >> (8*(1-j)); n++; }
        pblob[i-(n*2)-1] = (n*2);
            
/*      Encode the Current State */
        pblob[i++] = 6;
        n = i;
        pblob[i++] = 0;
        for (j=0; j<4; j++) 
            {
            if ((((*Maneuver[m].data).state & (0xff000000 >> (8*j))) != 0) || (pblob[n] > 0)) 
                {
                pblob[i++] = (unsigned char)(((*Maneuver[m].data).state & (0xff000000 >> (8*j))) >> (8*(3-j)));
                pblob[n] = pblob[n] + 1;
                };
            }            

/*      Encode the Minimum Time Remaining */
        pblob[i++] = 7;
        pblob[i++] = 2;
        for (j=0; j<2; j++) pblob[i++] = (spat_clip((*Maneuver[m].data).mintime) & (0xff00 >> (8*j))) >> (8*(1-j));

/*      Encode the Maximum Time Remaining */
        pblob[i++] = 8;
        pblob[i++] = 2;
        for (j=0; j<2; j++) pblob[i++] = (spat_clip((*Maneuver[m].data).maxtime) & (0xff00 >> (8*j))) >> (8*(1-j));

/*      Encode the Yellow State */
        if ((*Maneuver[m].data).yellowstate != 0)
            {
            pblob[i++] = 9;
            n = i;
            pblob[i++] = 0;
            for (j=0; j<4; j++) 
                {
                if ((((*Maneuver[m].data).yellowstate & (0xff000000 >> (8*j))) != 0) || (pblob[n] > 0)) 
                    {
                    pblob[i++] = (unsigned char)(((*Maneuver[m].data).yellowstate & (0xff000000 >> (8*j))) >> (8*(3-j)));
                    pblob[n] = pblob[n] + 1;
                    };
                }
                
/*          Encode the Yellow Time */
            pblob[i++] = 10;
            pblob[i++] = 2;
            for (j=0; j<2; j++) pblob[i++] = (spat_clip((*Maneuver[m].data).yellowtime) & (0xff00 >> (8*j))) >> (8*(1-j));
            }

/*      Encode the Pedestrian Detect */
        if ((*Maneuver[m].data).pedestrian != unavailable)
            {
            pblob[i++] = 11;
            pblob[i++] = 1;
            pblob[i++] = (*Maneuver[m].data).pedestrian;
            }
            
/*      Encode the Count */
        if ((*Maneuver[m].data).count != 0)
            {
            pblob[i++] = 12;
            pblob[i++] = 1;
            pblob[i++] = (*Maneuver[m].data).count;
            }
        }

/*  End of Blob */
    pblob[i++] = 255;

/*  Record the Message Size */
    for (j=0; j<2; j++) pblob[s++] = ((i-4) & (0xff00 >> (8*j))) >> (8*(1-j));

/*  Calculate the CRC Value */
    crc = crc_ccitt(pblob, i);    
    for (j=0; j<2; j++) pblob[i++] = (crc & (0xff00 >> (8*j))) >> (8*(1-j));

/*  Resize the SPAT Blob  */
    pblob = (unsigned char*) realloc(pblob, i);
    
/*  Return the Blob and Size */        
    (*message).payload.pblob = pblob;
    (*message).payload.size = i;
    
/*  Return TRUE */
    return 1;
    }   
     
/*************************************************************************************
*  Name       : spat_decode 
*  Purpose    : Decode the blob payload into the SPAT message
*  Parameters : message  - Pointer to the SPAT message structure
*************************************************************************************/
int spat_decode(spat *message)
    {
	int isize, icount, j, m, c;
	unsigned int i;
    unsigned char *pblob;

/*  Exit if there is no Blob to Decode */
    if ((*message).payload.pblob == NULL) 
        { 
        return 0;
        }  
        
/*  Clear any Previous Data */
    spat_clear(message);
    pblob = (*message).payload.pblob;

/*  Parse the SPAT Blob */
    i = 4;
    m = -1;
    icount = 0;
    while ((pblob[i] != 255) && (i<(*message).payload.size-2) && (m<spat_maxmovements))
        {

/*      Exit if Movement Data Objects are Not in a Movement */
        if ((pblob[i]>4) && (m<0))
            { 
            return 0;
            }  
    
/*      Decode Each Data Object */
        switch (pblob[i++]) 
            {

/*          Decode the Intersection ID */
            case 1:
            i++;
            for (j=0; j<4; j++) (*message).intersectionid |= pblob[i++] << (8*(3-j));
            break;
            
/*          Decode the Intersection Status */
            case 2: 
            i++;
            (*message).intersectionstatus = pblob[i++];
            break;

/*          Decode the Message Timestamp */
            case 3:
            i++;
            for (j=0; j<4; j++) (*message).timestampseconds |= pblob[i++] << (8*(3-j));
            (*message).timestamptenths = pblob[i++];
            break;

/*          Decode a Movement */
            case 4:
			c = icount;
            while (icount > 1)
                {
                icount--;
                spat_copy(&(*message).movement[m+icount].data, &(*message).movement[m].data);                
                }
			if (c > 0) {m += c;} else {m++;}
            break;    
    
/*          Decode the Lane Set */
            case 5:
            icount = spat_decompile(i, pblob, &(*message).movement[m]);
            i += pblob[i] + 1;            
            break; 
                           
/*          Decode the Current State */
            case 6:
            isize = pblob[i++];
            for (j=0; j<isize; j++) (*message).movement[m].data.state |= pblob[i++] << (8*((isize-1)-j));
            break;                
            
/*          Decode the Minimum Time Remaining */
            case 7:
            i++;
            for (j=0; j<2; j++) (*message).movement[m].data.mintime |= pblob[i++] << (8*(1-j));
            break;

/*          Decode the Maximum Time Remaining */
            case 8:
            i++;
            for (j=0; j<2; j++) (*message).movement[m].data.maxtime |= pblob[i++] << (8*(1-j));
            break;

/*          Decode the Yellow State */
            case 9:
            isize = pblob[i++];
            for (j=0; j<isize; j++) (*message).movement[m].data.yellowstate |= pblob[i++] << (8*((isize-1)-j));
            break;
            
/*          Decode the Yellow Time */
            case 10:
            i++;
            for (j=0; j<2; j++) (*message).movement[m].data.yellowtime |= pblob[i++] << (8*(1-j));
            break;

/*          Decode the Pedestrian Detect */
            case 11:
            i++;
            (*message).movement[m].data.pedestrian = pblob[i++];
            break;
            
/*          Decode the Vehicle or Pedestrian Count */
            case 12:
            i++;
            (*message).movement[m].data.count = pblob[i++];
            break;    
            }
        }    
    
/*  Return TRUE */
    return 1;
    }

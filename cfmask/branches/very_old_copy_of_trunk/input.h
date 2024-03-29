#ifndef INPUT_H
#define INPUT_H

#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include "myhdf.h"
#include "const.h"
#include "date.h"
#include "error.h"
#include "mystring.h"
#include "ias_types.h"
#include "cfmask.h"

/* Structure for the metadata */
typedef struct {
  char provider[MAX_STR_LEN];  /* Data provider type */
  char sat[MAX_STR_LEN];       /* Satellite */
  char inst[MAX_STR_LEN];      /* Instrument */
  Date_t acq_date;             /* Acqsition date/time (scene center) */
  Date_t prod_date;            /* Production date (must be available for ETM) */
  float sun_zen;               /* Solar zenith angle (radians; scene center) */
  float sun_az;                /* Solar azimuth angle (radians; scene center) */
  char wrs_sys[MAX_STR_LEN];   /* WRS system */
  char unit_ref[MAX_STR_LEN];  
  char therm_unit_ref[MAX_STR_LEN];  
  int path;                    /* WRS path number */
  int row;                     /* WRS row number */
  int fill;                    /* Fill value for image data */
  int zone;
  float valid_range_ref[2];
  float satu_value_ref; 
  float scale_factor_ref; 
  float add_offset_ref; 
  float scale_factor_err_ref; 
  float add_offset_err_ref; 
  float calibrated_nt_ref; 
  float therm_valid_range_ref[2];
  float therm_satu_value_ref; 
  float therm_scale_factor_ref; 
  float therm_add_offset_ref; 
  float therm_scale_factor_err_ref; 
  float therm_add_offset_err_ref; 
  float therm_calibrated_nt_ref; 
  float ul_lat;
  float ul_lon;
  float ul_projection_x;
  float ul_projection_y;
  int band[NBAND_REFL_MAX];    /* Band numbers */
} Input_meta_t;

/* Structure for the 'input' data type */
typedef struct {
  char *lndsr_name;        /* Input surface reflecetance image file name */
  char *lndcal_name;       /* Input TOA reflectance image file name */
  bool open;               /* Open file flag; open = true */
  Input_meta_t meta;       /* Input metadata */
  int nband;               /* Number of input image bands */
  Img_coord_int_t size;    /* Input file size */
  Img_coord_int_t toa_size;   /* Input file size */
  int32 sds_sr_file_id;       /* SDS file id */
  int32 sds_cal_file_id;       /* SDS file id */
  Myhdf_sds_t sds[NBAND_REFL_MAX];
                           /* SDS data structures for TOA image data */
  int16 *buf[NBAND_REFL_MAX];
                           /* Input data buffer (one line of image data) */
  Myhdf_sds_t therm_sds;   /* SDS data structure for thermal image data */
  int16 *therm_buf;        /* Input data buffer (one line of thermal data) */
} Input_t;

typedef struct {
  char *param_file_name;         /* Parameter file name                */
  char *input_header_file_name;  /* Input image header file name       */
  char *lut_file_name;           /* Lookup table file name             */
  char *work_order_file_name;    /* Work Order (wo) file name          */
  char *gold_file_name;          /* G-old file name                    */
  char *gold_2003_name;          /* G-old file name                    */
  char *gnew_file_name;          /* G-new file name                    */
  char *output_file_name;        /* Output image HDF file name         */
  char *output_therm_file_name;  /* Output thermal image HDF file name */
  char *PGEVersion;              /* PGE Version                        */
  char *ProcessVersion;          /* Process Version                    */
  float dn_map[4];               /* map from(0,1) -> (2,3)             */
  bool dnout;                    /* dn output flag                     */
  bool work_order_flag;          /* work order input flag              */
  bool gold_flag;                /* G-old input flag                   */
  bool gold_2003_flag;           /* G-old input flag                   */
  bool gnew_flag;                /* G-new input flag                   */
  bool ETM_GB;                   /* ETM_GB flag                        */
  bool RE_CAL;                   /* re-calibration flag  (fgao added)  */             
  int  est_gainbias;             /* estimate Gain/bias?  1=yes         */
} Param_t;

typedef enum {
  WRS_NULL = -1,
  WRS_1 = 0, 
  WRS_2,
  WRS_MAX
} Wrs_t;

/* Prototypes */
Input_t *OpenInput(char *lndsr_name, char *lndcal_name, char *lndmeta_name);
bool GetInputLine(Input_t *this, int iband, int iline);
bool GetInputQALine(Input_t *this, int iband, int iline);
bool GetInputThermLine(Input_t *this, int iline);
bool CloseInput(Input_t *this);
bool FreeInput(Input_t *this);
bool GetInputMeta(Input_t *this);
bool GetInputMeta2(Input_t *this);
bool GetInputMeta3(Input_t *this);
bool GetHeaderInput(Input_t *this, char *file_header_name, Param_t *param);

bool potential_cloud_shadow_snow_mask
(
  Input_t *input,
  float cloud_prob_threshold,
  float *ptm,
  float *t_templ,
  float *t_temph,
  unsigned char **cloud_mask,
  unsigned char **shadow_mask,
  unsigned char **snow_mask,
  unsigned char **water_mask,
  unsigned char **final_mask
);

int object_cloud_shadow_match
(
  Input_t *input,
  float ptm,
  float t_templ,
  float t_temph,
  int cldpix,
  int sdpix,
  unsigned char **cloud_mask,
  unsigned char **shadow_mask,
  unsigned char **snow_mask,
  unsigned char **water_mask,
  unsigned char **final_mask
);

int ias_misc_write_envi_header
(
    const char *image_filename, /* I: Full path name of the image file */
    const IAS_PROJECTION *proj_info, /* I: Optional projection info, set to 
                                           NULL if not known or needed */
    const char *description,    /* I: Optional description, set to NULL if not 
                                      known or needed */
    int lines,                  /* I: Number of lines in the data */
    int samples,                /* I: Number of samples in the data */
    int bands,                  /* I: Number of bands in the data */
    double upper_left_x,        /* I: Optional upper-left X coordinate, set to 
                                      0.0 if not known or needed (requires
                                      proj_info) */
    double upper_left_y,        /* I: Optional upper-left Y coordinate, set to 
                                      0.0 if not known or needed (requires
                                      proj_info) */
    double projection_distance_x, /* Optional pixel size in X projection, set
                                     to 0.0 if not known or needed (requires
                                     proj_info) */
    double projection_distance_y, /* Optional pixel size in Y projection, set 
                                     to 0.0 if not known or needed (requires
                                     proj_info) */
    IAS_DATA_TYPE data_type     /* I: The IAS type of the data */
);

void ias_misc_split_filename 
(
    const char *filename,       /* I: Name of file to split */
    char *directory,            /* O: Directory portion of file name */
    char *root,                 /* O: Root portion of the file name */
    char *extension             /* O: Extension portion of the file name */
);

void prctile
(
 int16 *array,
 int nums, 
 float prct,
 float *result
);

void prctile2
(
 float *array,
 int nums, 
 float prct,
 float *result
);

#endif

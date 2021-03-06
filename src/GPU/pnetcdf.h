/*
 * Copyright 1993-1996 University Corporation for Atmospheric Research/Unidata
 * 
 * Portions of this software were developed by the Unidata Program at the 
 * University Corporation for Atmospheric Research.
 * 
 * Access and use of this software shall impose the following obligations
 * and understandings on the user. The user is granted the right, without
 * any fee or cost, to use, copy, modify, alter, enhance and distribute
 * this software, and any derivative works thereof, and its supporting
 * documentation for any purpose whatsoever, provided that this entire
 * notice appears in all copies of the software, derivative works and
 * supporting documentation.  Further, UCAR requests that the user credit
 * UCAR/Unidata in any publications that result from the use of this
 * software or in any product that includes this software. The names UCAR
 * and/or Unidata, however, may not be used in any advertising or publicity
 * to endorse or promote any products or commercial entity unless specific
 * written permission is obtained from UCAR/Unidata. The user also
 * understands that UCAR/Unidata is not obligated to provide the user with
 * any support, consulting, training or assistance of any kind with regard
 * to the use, operation and performance of this software nor to provide
 * the user with any updates, revisions, new versions or "bug fixes."
 * 
 * THIS SOFTWARE IS PROVIDED BY UCAR/UNIDATA "AS IS" AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL UCAR/UNIDATA BE LIABLE FOR ANY SPECIAL,
 * INDIRECT OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING
 * FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT,
 * NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION
 * WITH THE ACCESS, USE OR PERFORMANCE OF THIS SOFTWARE.
 */
/* "$Id: pnetcdf.h.in 1087 2012-08-16 11:17:06Z wkliao $" */

#ifndef _PNETCDF_
#define _PNETCDF_

#include <stddef.h> /* size_t, ptrdiff_t */
#include <errno.h>  /* netcdf functions sometimes return system errors */
#include <mpi.h>
#include <stdint.h>

#if defined(__cplusplus)
extern "C" {
#endif

#ifndef PNETCDF_VERSION_MAJOR
#define PNETCDF_VERSION_MAJOR 1
#define PNETCDF_VERSION_MINOR 3
#define PNETCDF_VERSION_SUB 1
#define PNETCDF_VERSION_PRE 
#endif

/*
 *  The netcdf external data types
 */
	/* we happen to like enums because you can print them in the debugger
	 * but guard against the case where netcdf already defines these and
	 * includes pnetcdf (as netcdf4 can) */
#ifndef _NETCDF_
typedef enum {
    NC_NAT    = 0,  /* NAT = 'Not A Type' (c.f. NaN) */
    NC_BYTE   = 1,  /* signed 1 byte integer */
    NC_CHAR   = 2,  /* ISO/ASCII character */
    NC_SHORT  = 3,  /* signed 2 byte integer */
    NC_INT    = 4,  /* signed 4 byte integer */
    NC_FLOAT  = 5,  /* single precision floating point number */
    NC_DOUBLE = 6,  /* double precision floating point number */
    NC_UBYTE  = 7,  /* unsigned 1 byte int */
    NC_USHORT = 8,  /* unsigned 2-byte int */
    NC_UINT   = 9,  /* unsigned 4-byte int */
    NC_INT64  = 10, /* signed 8-byte int */
    NC_UINT64 = 11, /* unsigned 8-byte int */
    NC_STRING = 12  /* string */
} nc_type;
#define NC_LONG NC_INT; /* deprecated, but required for backward compati bility. */

/* corresponding external types and type names used in APIs:
    NC_BYTE   :   signed char       (for _schar     APIs)
    NC_CHAR   :   signed char       (for _text      APIs)
    NC_SHORT  :   signed short int  (for _short     APIs)
    NC_INT    :   signed int        (for _int       APIs)
    NC_FLOAT  :          float      (for _float     APIs)
    NC_DOUBLE :          double     (for _double    APIs)
    NC_UBYTE  : unsigned char       (for _ubyte and
                                         _uchar     APIs)
    NC_USHORT : unsigned short int  (for _ushort    APIs)
    NC_UINT   : unsigned int        (for _uint      APIs)
    NC_INT64  :   signed long long  (for _longlong  APIs)
    NC_UINT64 : unsigned long long  (for _ulonglong APIs)
    NC_STRING :   signed char       (for _string    APIs)
 */
#endif

/* typedef nc_type ncmpi_type; */
/* ncmpi_type is deprecated (hopefully no one has ever used it)
 * it shall be removed */

/*
 * 	Default fill values, used unless _FillValue attribute is set.
 * These values are stuffed into newly allocated space as appropriate.
 * The hope is that one might use these to notice that a particular datum
 * has not been set.
 */
#define NC_FILL_BYTE	((signed char)-127)
#define NC_FILL_CHAR	((char)0)
#define NC_FILL_SHORT	((short)-32767)
#define NC_FILL_INT	(-2147483647L)
#define NC_FILL_FLOAT	(9.9692099683868690e+36f) /* near 15 * 2^119 */
#define NC_FILL_DOUBLE	(9.9692099683868690e+36)

#define NC_FILL_UBYTE   (255)
#define NC_FILL_USHORT  (65535)
#define NC_FILL_UINT    (4294967295U)
#define NC_FILL_INT64   ((long long)-9223372036854775806LL)
#define NC_FILL_UINT64  ((unsigned long long)18446744073709551614ULL)
#define NC_FILL_STRING  ""


/*
 * The above values are defaults.
 * If you wish a variable to use a different value than the above
 * defaults, create an attribute with the same type as the variable
 * and the following reserved name. The value you give the attribute
 * will be used as the fill value for that variable.
 */
/* do not redefine these if we are included from netcdf */
#ifndef _NETCDF_
#define _FillValue	"_FillValue"
#define NC_FILL		0	/* argument to ncsetfill to clear NC_NOFILL */
#define NC_NOFILL	0x0100	/* Don't fill data section an records */


/*
 * 'mode' flags for ncmpi_open
 */
#define NC_NOWRITE	0	/* default is read only */
#define NC_WRITE    	0x0001	/* read & write */

/* mode flags for ncmpi_create */
#define NC_CLOBBER	0
#define NC_NOCLOBBER	0x0004	/* Don't destroy existing file on create */
#define NC_64BIT_OFFSET 0x0200  /* Use large (64-bit) file offsets */
#define NC_LOCK		0x0400	/* Use locking if available */
#define NC_SHARE	0x0800	/* Share updates, limit caching */
#if 0
/* these are used by netcdf4: play nice and do not use these flags */
        NC_NETCDF4      0x1000   used by netcdf4 to select new format */
	NC_MPIIO        0x2000
	NC_MPIPOSIX     0x4000
	NC_PNETCDF      0x8000
#endif

#define NC_32BIT        0x1000000 /* pnetcdf version 1: this is used internally
				     and never actually passed in to
				     ncmpi_create  */
#define NC_64BIT_DATA   0x0010  /* (64-bit) supported */

/* Starting with serial netcdf-3.6 and parallel-netcdf-0.9.2, there are
 * different format netCDF files */

#define NC_FORMAT_CLASSIC 1
#define NC_FORMAT_64BIT 2
#define NC_FORMAT_UNKNOWN -1
#define NC_FORMAT_64BIT_DATA 5

#endif

/*
 * Let nc__create() or nc__open() figure out
 * as suitable chunk size.
 */
#define NC_SIZEHINT_DEFAULT 0

/*
 * In nc__enddef(), align to the chunk size.
 */
#define NC_ALIGN_CHUNK ((size_t)(-1))

/*
 * 'size' argument to ncdimdef for an unlimited dimension
 */
#define NC_UNLIMITED 0L

/*
 * attribute id to put/get a global attribute
 */
#define NC_GLOBAL -1


/*
 * These maximums are enforced by the interface, to facilitate writing
 * applications and utilities.  However, nothing is statically allocated to
 * these sizes internally.
 */
#ifndef _NETCDF_
#define NC_MAX_DIMS	512    /* max dimensions per file */
#define NC_MAX_ATTRS	4096	 /* max global or per variable attributes */
#define NC_MAX_VARS	4096	 /* max variables per file */
#define NC_MAX_NAME	128	 /* max length of a name */
#define NC_MAX_VAR_DIMS	NC_MAX_DIMS /* max per variable dimensions */
#endif


/*
 * The netcdf version 3 functions all return integer error status.
 * These are the possible values, in addition to certain
 * values from the system errno.h.
 */

#define NC_ISSYSERR(err)	((err) > 0)

#define NC_NOERR	0	/* No Error */

#define NC2_ERR         (-1)    /* Returned for all errors in the v2 API. */

/* NetCDF-3 Error Codes */
#define NC_EBADID	(-33) /* Not a netcdf id */
#define NC_ENFILE	(-34) /* Too many netcdfs open */
#define NC_EEXIST	(-35) /* netcdf file exists && NC_NOCLOBBER */
#define NC_EINVAL	(-36) /* Invalid Argument */
#define NC_EPERM	(-37) /* Write to read only */
#define NC_ENOTINDEFINE	(-38) /* Operation not allowed in data mode */
#define NC_EINDEFINE	(-39) /* Operation not allowed in define mode */
#define NC_EINVALCOORDS	(-40) /* Index exceeds dimension bound */
#define NC_EMAXDIMS	(-41) /* NC_MAX_DIMS exceeded */
#define NC_ENAMEINUSE	(-42) /* String match to name in use */
#define NC_ENOTATT	(-43) /* Attribute not found */
#define NC_EMAXATTS	(-44) /* NC_MAX_ATTRS exceeded */
#define NC_EBADTYPE	(-45) /* Not a netcdf data type */
#define NC_EBADDIM	(-46) /* Invalid dimension id or name */
#define NC_EUNLIMPOS	(-47) /* NC_UNLIMITED in the wrong index */
#define NC_EMAXVARS	(-48) /* NC_MAX_VARS exceeded */
#define NC_ENOTVAR	(-49) /* Variable not found */
#define NC_EGLOBAL	(-50) /* Action prohibited on NC_GLOBAL varid */
#define NC_ENOTNC	(-51) /* Not a netcdf file */
#define NC_ESTS		(-52) /* In Fortran, string too short */
#define NC_EMAXNAME	(-53) /* NC_MAX_NAME exceeded */
#define NC_EUNLIMIT	(-54) /* NC_UNLIMITED size already in use */
#define NC_ENORECVARS	(-55) /* nc_rec op when there are no record vars */
#define NC_ECHAR	(-56) /* Attempt to convert between text & numbers */
#define NC_EEDGE	(-57) /* Edge+start exceeds dimension bound */
#define NC_ESTRIDE	(-58) /* Illegal stride */
#define NC_EBADNAME	(-59) /* Attribute or variable name contains illegal characters */

/* N.B. following must match value in ncx.h */
#define NC_ERANGE	(-60) /* Math result not representable */
#define NC_ENOMEM	(-61) /* Memory allocation (malloc) failure */
#define NC_EVARSIZE	(-62) /* One or more variable sizes violate format constraints */
#define NC_EDIMSIZE	(-63) /* Invalid dimension size */
#define NC_ETRUNC	(-64) /* File likely truncated or possibly corrupted */

/* NetCDF-4 Error Codes (provide here but not used in PnetCDF) */
/* The following was added in support of netcdf-4. Make all netcdf-4
   error codes < -100 so that errors can be added to netcdf-3 if
   needed.
 */
#ifndef NC4_FIRST_ERROR
#define NC4_FIRST_ERROR	(-100)
#define NC_EHDFERR	(-101) /* Error at HDF5 layer. */
#define NC_ECANTREAD	(-102) /* Can't read. */
#define NC_ECANTWRITE	(-103) /* Can't write. */
#define NC_ECANTCREATE	(-104) /* Can't create. */
#define NC_EFILEMETA	(-105) /* Problem with file metadata. */
#define NC_EDIMMETA	(-106) /* Problem with dimension metadata. */
#define NC_EATTMETA	(-107) /* Problem with attribute metadata. */
#define NC_EVARMETA	(-108) /* Problem with variable metadata. */
#define NC_ENOCOMPOUND	(-109) /* Not a compound type. */
#define NC_EATTEXISTS	(-110) /* Attribute already exists. */
#define NC_ENOTNC4	(-111) /* Attempting netcdf-4 operation on netcdf-3 file. */  
#define NC_ESTRICTNC3	(-112) /* Attempting netcdf-4 operation on strict nc3 netcdf-4 file. */  
#define NC_ENOTNC3	(-113) /* Attempting netcdf-3 operation on netcdf-4 file. */  
#define NC_ENOPAR	(-114) /* Parallel operation on file opened for non-parallel access. */  
#define NC_EPARINIT	(-115) /* Error initializing for parallel access. */  
#define NC_EBADGRPID	(-116) /* Bad group ID. */  
#define NC_EBADTYPID	(-117) /* Bad type ID. */  
#define NC_ETYPDEFINED	(-118) /* Type has already been defined and may not be edited. */
#define NC_EBADFIELD	(-119) /* Bad field ID. */  
#define NC_EBADCLASS	(-120) /* Bad class. */  
#define NC_EMAPTYPE	(-121) /* Mapped access for atomic types only. */  
#define NC_ELATEFILL	(-122) /* Attempt to define fill value when data already exists. */
#define NC_ELATEDEF	(-123) /* Attempt to define var properties, like deflate, after enddef. */
#define NC_EDIMSCALE	(-124) /* Probem with HDF5 dimscales. */
#define NC_ENOGRP	(-125) /* No group found. */
#define NC_ESTORAGE	(-126) /* Can't specify both contiguous and chunking. */
#define NC_EBADCHUNK	(-127) /* Bad chunksize. */
#define NC_ENOTBUILT	(-128) /* Attempt to use feature that was not turned on when netCDF was built. */
#define NC_EDISKLESS    (-129) /**< Error in using diskless  access. */  
#define NC4_LAST_ERROR	(-129) 
#endif

/* PnetCDF Error Codes: */
#define NC_ESMALL			(-201) /* size of off_t too small for format */
#define NC_ENOTINDEP			(-202) /* Operation not allowed in collective data mode */
#define NC_EINDEP			(-203) /* Operation not allowed in independent data mode */
#define NC_EFILE			(-204) /* Unknown error in file operation */
#define NC_EREAD			(-205) /* Unknown error in reading file */
#define NC_EWRITE			(-206) /* Unknown error in writting to file */
#define NC_EMULTIDEFINE			(-207) /* NC definitions on multiprocesses conflict */
#define NC_EOFILE			(-208) /* file open/creation failed */
#define NC_EMULTITYPES			(-209) /* Multiple types used in memory data */
#define NC_EIOMISMATCH			(-210) /* Input/Output data amount mismatch */
#define NC_ENEGATIVECNT			(-211) /* Negative count is specified */
#define NC_EUNSPTETYPE			(-212) /* Unsupported etype in memory MPI datatype */
#define NC_EDIMS_NELEMS_MULTIDEFINE	(-213) /* Different number of dim defines on multiprocesses conflict */
#define NC_EDIMS_SIZE_MULTIDEFINE	(-214) /* Different size of dim defines on multiprocesses conflict */
#define NC_EVARS_NELEMS_MULTIDEFINE	(-215) /* Different number of var defines on multiprocesses conflict */
#define NC_EVARS_NDIMS_MULTIDEFINE	(-216) /* Different dim number of var defines on multiprocesses conflict */
#define NC_EVARS_DIMIDS_MULTIDEFINE	(-217) /* Different dimid defines on multiprocesses conflict */
#define NC_EVARS_TYPE_MULTIDEFINE	(-218) /* Different type of var defines on multiprocesses conflict */
#define NC_EVARS_LEN_MULTIDEFINE	(-219) /* Different var lenght defines size on multiprocesses conflict */
#define NC_EVARS_BEGIN_MULTIDEFINE	(-220) /* Different var begin defines size on multiprocesses conflict */
#define NC_ENUMRECS_MULTIDEFINE		(-221) /* Different number records on multiprocesses conflict */
#define NC_EINVAL_REQUEST		(-222) /* invalid nonblocking request ID */
#define NC_EAINT_TOO_SMALL		(-223) /* MPI_Aint not large enough to hold requested value */
#define NC_ECMODE			(-224) /* file create modes are inconsistent among processes */
#define NC_ENOTSUPPORT			(-225) /* feature is not yet supported */
#define NC_ENULLBUF			(-226) /* trying to attach a NULL buffer */
#define NC_EPREVATTACHBUF		(-227) /* previous attached buffer is found */
#define NC_ENULLABUF			(-228) /* no attached buffer is found */
#define NC_EPENDINGBPUT			(-229) /* pending bput is found, cannot detach buffer */
#define NC_EINSUFFBUF			(-230) /* attached buffer is too small */
#define NC_ENOENT			(-231) /* File does not exist when calling ncmpi_open() */

#define NC_REQ_NULL -1  /* zeor-length nonblocking request will be ignored */


/*
 * The Interface
 */

/* Begin Prototypes */

const char* ncmpi_strerror(int err);

/* Begin Dataset Functions */

int ncmpi_create(MPI_Comm comm, const char *path, int cmode, MPI_Info info, int *ncidp); 

int ncmpi_open(MPI_Comm comm, const char *path, int omode, MPI_Info info, int *ncidp);

int ncmpi_get_file_info(int ncid, MPI_Info *info_used);

int ncmpi_delete(char *filename, MPI_Info info);

int ncmpi_enddef(int ncid);

int ncmpi_redef(int ncid);

int ncmpi_set_default_format(int format, int *old_formatp);

int ncmpi_sync(int ncid);

int ncmpi_abort(int ncid);

int ncmpi_begin_indep_data(int ncid);

int ncmpi_end_indep_data(int ncid);

int ncmpi_close(int ncid);

int ncmpi_set_fill(int ncid, int fillmode, int *old_modep);

/* End Dataset Functions */

/* Begin Define Mode Functions */

int ncmpi_def_dim(int ncid, const char *name, MPI_Offset len, int *idp);

int ncmpi_def_var(int ncid, const char *name, nc_type xtype, 
                  int ndims, const int *dimidsp, int *varidp);

int ncmpi_rename_dim(int ncid, int dimid, const char *name);

int ncmpi_rename_var(int ncid, int varid, const char *name);

/* End Define Mode Functions */

/* Begin Inquiry Functions */

const char* ncmpi_inq_libvers(void);

int ncmpi_inq(int ncid, int *ndimsp, int *nvarsp,
              int *ngattsp, int *unlimdimidp); 

int ncmpi_inq_format(int ncid, int *formatp);

int ncmpi_inq_file_format(char *filename, int *formatp);

int ncmpi_inq_version(int ncid, int *NC_mode);

int ncmpi_inq_ndims(int ncid, int *ndimsp);

int ncmpi_inq_nvars(int ncid, int *nvarsp);

int ncmpi_inq_natts(int ncid, int *ngattsp);

int ncmpi_inq_unlimdim(int ncid, int *unlimdimidp);

int ncmpi_inq_dimid(int ncid, const char *name, int *idp);

int ncmpi_inq_dim(int ncid, int dimid, char *name, MPI_Offset *lenp);

int ncmpi_inq_dimname(int ncid, int dimid, char *name);

int ncmpi_inq_dimlen(int ncid, int dimid, MPI_Offset *lenp);

int ncmpi_inq_var(int ncid, int varid, char *name,
                  nc_type *xtypep, int *ndimsp, int *dimidsp,
                  int *nattsp);

int ncmpi_inq_varid(int ncid, const char *name, int *varidp);

int ncmpi_inq_varname(int ncid, int varid, char *name);

int ncmpi_inq_vartype(int ncid, int varid, nc_type *xtypep);

int ncmpi_inq_varndims(int ncid, int varid, int *ndimsp);

int ncmpi_inq_vardimid(int ncid, int varid, int *dimidsp);

int ncmpi_inq_varnatts(int ncid, int varid, int *nattsp);

int ncmpi_inq_varoffset(int ncid, int varid, MPI_Offset *offset);

/* End Inquiry Functions */

/* Begin _att */

int ncmpi_inq_att(int ncid, int varid, const char *name,
                  nc_type *xtypep, MPI_Offset *lenp);

int ncmpi_inq_attid(int ncid, int varid, const char *name, int *idp);

int ncmpi_inq_atttype(int ncid, int varid, const char *name,
                      nc_type *xtypep);

int ncmpi_inq_attlen(int ncid, int varid, const char *name,
                     MPI_Offset *lenp);

int ncmpi_inq_attname(int ncid, int varid, int attnum, char *name);

int ncmpi_copy_att(int ncid_in, int varid_in, const char *name,
                   int ncid_out, int varid_out);

int ncmpi_rename_att(int ncid, int varid, const char *name,
                     const char *newname);

int ncmpi_del_att(int ncid, int varid, const char *name);

int ncmpi_put_att_text(int ncid, int varid, const char *name, MPI_Offset len,
                  const char *op);

int ncmpi_put_att_schar(int ncid, int varid, const char *name,
                  nc_type xtype, MPI_Offset len, const signed char *op);

int ncmpi_put_att_short(int ncid, int varid, const char *name,
                  nc_type xtype, MPI_Offset len, const short *op);

int ncmpi_put_att_int(int ncid, int varid, const char *name,
                  nc_type xtype, MPI_Offset len, const int *op);

int ncmpi_put_att_float(int ncid, int varid, const char *name,
                  nc_type xtype, MPI_Offset len, const float *op);

int ncmpi_put_att_double(int ncid, int varid, const char *name,
                  nc_type xtype, MPI_Offset len, const double *op);

int ncmpi_put_att_longlong(int ncid, int varid, const char *name,
                  nc_type xtype, MPI_Offset len, const long long *op);

int ncmpi_get_att_text(int ncid, int varid, const char *name, char *ip);

int ncmpi_get_att_schar(int ncid, int varid, const char *name,
                  signed char *ip);

int ncmpi_get_att_short(int ncid, int varid, const char *name, short *ip);

int ncmpi_get_att_int(int ncid, int varid, const char *name, int *ip);

int ncmpi_get_att_float(int ncid, int varid, const char *name, float *ip);

int ncmpi_get_att_double(int ncid, int varid, const char *name, double *ip);

int ncmpi_get_att_longlong(int ncid, int varid, const char *name, long long *ip);

/* Begin Skip Prototypes for Fortran binding */

int ncmpi_put_att_uchar(int ncid, int varid, const char *name,
                  nc_type xtype, MPI_Offset len, const unsigned char *op);

int ncmpi_put_att_ushort(int ncid, int varid, const char *name,
                  nc_type xtype, MPI_Offset len, const unsigned short *op);

int ncmpi_put_att_uint(int ncid, int varid, const char *name,
                  nc_type xtype, MPI_Offset len, const unsigned int *op);

int ncmpi_put_att_long(int ncid, int varid, const char *name,
                  nc_type xtype, MPI_Offset len, const long *op);

int ncmpi_put_att_ulonglong(int ncid, int varid, const char *name,
                  nc_type xtype, MPI_Offset len, const unsigned long long *op);

int ncmpi_get_att_uchar(int ncid, int varid, const char *name,
                  unsigned char *ip);

int ncmpi_get_att_ushort(int ncid, int varid, const char *name,
                  unsigned short *ip);

int ncmpi_get_att_uint(int ncid, int varid, const char *name,
                  unsigned int *ip);

int ncmpi_get_att_long(int ncid, int varid, const char *name,
                  long *ip);

int ncmpi_get_att_ulonglong(int ncid, int varid, const char *name,
                  unsigned long long *ip);
/* End Skip Prototypes for Fortran binding */

/* End _att */

/* Begin {put,get}_var1 */

int ncmpi_put_var1(int ncid, int varid, const MPI_Offset index[],
                   const void *buf, MPI_Offset bufcount, MPI_Datatype buftype);

int ncmpi_put_var1_text(int ncid, int varid, const MPI_Offset index[],
                   const char *op);

int ncmpi_put_var1_schar(int ncid, int varid, const MPI_Offset index[],
                   const signed char *op);

int ncmpi_put_var1_short(int ncid, int varid, const MPI_Offset index[],
                   const short *op);

int ncmpi_put_var1_int(int ncid, int varid, const MPI_Offset index[],
                   const int *op);

int ncmpi_put_var1_float(int ncid, int varid, const MPI_Offset index[],
                   const float *op);

int ncmpi_put_var1_double(int ncid, int varid, const MPI_Offset index[],
                   const double *op);

int ncmpi_put_var1_longlong(int ncid, int varid, const MPI_Offset index[],
                   const long long *op);

int ncmpi_get_var1(int ncid, int varid, const MPI_Offset index[],
                   void *buf, MPI_Offset bufcount, MPI_Datatype buftype);

int ncmpi_get_var1_text(int ncid, int varid, const MPI_Offset index[],
                   char *ip);

int ncmpi_get_var1_schar(int ncid, int varid, const MPI_Offset index[],
                   signed char *ip);

int ncmpi_get_var1_short(int ncid, int varid, const MPI_Offset index[],
                   short *ip);

int ncmpi_get_var1_int(int ncid, int varid, const MPI_Offset index[],
                   int *ip);

int ncmpi_get_var1_float(int ncid, int varid, const MPI_Offset index[],
                   float *ip);

int ncmpi_get_var1_double(int ncid, int varid, const MPI_Offset index[],
                   double *ip);

int ncmpi_get_var1_longlong(int ncid, int varid, const MPI_Offset index[],
                   long long *ip);

/* Begin Skip Prototypes for Fortran binding */

int ncmpi_put_var1_uchar(int ncid, int varid, const MPI_Offset index[],
                   const unsigned char *op);

int ncmpi_put_var1_ushort(int ncid, int varid, const MPI_Offset index[],
                   const unsigned short *op);

int ncmpi_put_var1_uint(int ncid, int varid, const MPI_Offset index[],
                   const unsigned int *op);

int ncmpi_put_var1_long(int ncid, int varid, const MPI_Offset index[],
                   const long *ip);

int ncmpi_put_var1_ulonglong(int ncid, int varid, const MPI_Offset index[],
                   const unsigned long long *ip);

int ncmpi_get_var1_uchar(int ncid, int varid, const MPI_Offset index[],
                   unsigned char *ip);

int ncmpi_get_var1_ushort(int ncid, int varid, const MPI_Offset index[],
                   unsigned short *ip);

int ncmpi_get_var1_uint(int ncid, int varid, const MPI_Offset index[],
                   unsigned int *ip);

int ncmpi_get_var1_long(int ncid, int varid, const MPI_Offset index[],
                   long *ip);

int ncmpi_get_var1_ulonglong(int ncid, int varid, const MPI_Offset index[],
                   unsigned long long *ip);
/* End Skip Prototypes for Fortran binding */

/* End {put,get}_var1 */

/* Begin {put,get}_var */  

int ncmpi_put_var(int ncid, int varid, const void *buf, MPI_Offset bufcount,
                  MPI_Datatype buftype);

int ncmpi_put_var_all(int ncid, int varid, const void *buf, MPI_Offset bufcount,
                  MPI_Datatype buftype);

int ncmpi_put_var_text(int ncid, int varid, const char *op);
int ncmpi_put_var_text_all(int ncid, int varid, const char *op);

int ncmpi_put_var_schar(int ncid, int varid, const signed char *op);
int ncmpi_put_var_schar_all(int ncid, int varid, const signed char *op);

int ncmpi_put_var_short(int ncid, int varid, const short *op);
int ncmpi_put_var_short_all(int ncid, int varid, const short *op);

int ncmpi_put_var_int(int ncid, int varid, const int *op);
int ncmpi_put_var_int_all(int ncid, int varid, const int *op);

int ncmpi_put_var_float(int ncid, int varid, const float *op);
int ncmpi_put_var_float_all(int ncid, int varid, const float *op);

int ncmpi_put_var_double(int ncid, int varid, const double *op);
int ncmpi_put_var_double_all(int ncid, int varid, const double *op);

int ncmpi_put_var_longlong(int ncid, int varid, const long long *op);
int ncmpi_put_var_longlong_all(int ncid, int varid, const long long *op);

int ncmpi_get_var(int ncid, int varid, void *buf, MPI_Offset bufcount,
                  MPI_Datatype buftype);

int ncmpi_get_var_all(int ncid, int varid, void *buf, MPI_Offset bufcount,
                  MPI_Datatype buftype);

int ncmpi_get_var_text(int ncid, int varid, char *ip);
int ncmpi_get_var_text_all(int ncid, int varid, char *ip);

int ncmpi_get_var_schar(int ncid, int varid, signed char *ip);
int ncmpi_get_var_schar_all(int ncid, int varid, signed char *ip);

int ncmpi_get_var_short(int ncid, int varid, short *ip);
int ncmpi_get_var_short_all(int ncid, int varid, short *ip);

int ncmpi_get_var_int(int ncid, int varid, int *ip);
int ncmpi_get_var_int_all(int ncid, int varid, int *ip);

int ncmpi_get_var_float(int ncid, int varid, float *ip);
int ncmpi_get_var_float_all(int ncid, int varid, float *ip);

int ncmpi_get_var_double(int ncid, int varid, double *ip);
int ncmpi_get_var_double_all(int ncid, int varid, double *ip);

int ncmpi_get_var_longlong(int ncid, int varid, long long *ip);
int ncmpi_get_var_longlong_all(int ncid, int varid, long long *ip);

/* Begin Skip Prototypes for Fortran binding */

int ncmpi_put_var_uchar(int ncid, int varid, const unsigned char *op);
int ncmpi_put_var_uchar_all(int ncid, int varid, const unsigned char *op);

int ncmpi_put_var_ushort(int ncid, int varid, const unsigned short *op);
int ncmpi_put_var_ushort_all(int ncid, int varid, const unsigned short *op);

int ncmpi_put_var_uint(int ncid, int varid, const unsigned int *op);
int ncmpi_put_var_uint_all(int ncid, int varid, const unsigned int *op);

int ncmpi_put_var_long(int ncid, int varid, const long *op);
int ncmpi_put_var_long_all(int ncid, int varid, const long *op);

int ncmpi_put_var_ulonglong(int ncid, int varid, const unsigned long long *op);
int ncmpi_put_var_ulonglong_all(int ncid, int varid, const unsigned long long *op);

int ncmpi_get_var_uchar(int ncid, int varid, unsigned char *ip);
int ncmpi_get_var_uchar_all(int ncid, int varid, unsigned char *ip);

int ncmpi_get_var_ushort(int ncid, int varid, unsigned short *ip);
int ncmpi_get_var_ushort_all(int ncid, int varid, unsigned short *ip);

int ncmpi_get_var_uint(int ncid, int varid, unsigned int *ip);
int ncmpi_get_var_uint_all(int ncid, int varid, unsigned int *ip);

int ncmpi_get_var_long(int ncid, int varid, long *ip);
int ncmpi_get_var_long_all(int ncid, int varid, long *ip);

int ncmpi_get_var_ulonglong(int ncid, int varid, unsigned long long *ip);
int ncmpi_get_var_ulonglong_all(int ncid, int varid, unsigned long long *ip);
/* End Skip Prototypes for Fortran binding */

/* End {put,get}_var */

/* Begin {put,get}_vara */

int ncmpi_put_vara(int ncid, int varid, const MPI_Offset start[],
                   const MPI_Offset count[], const void *buf,
                   MPI_Offset bufcount, MPI_Datatype buftype);

int ncmpi_put_vara_all(int ncid, int varid, const MPI_Offset start[],
                   const MPI_Offset count[], const void *buf,
                   MPI_Offset bufcount, MPI_Datatype buftype);

int ncmpi_put_vara_text(int ncid, int varid, const MPI_Offset start[],
                   const MPI_Offset count[], const char *op);

int ncmpi_put_vara_text_all(int ncid, int varid, const MPI_Offset start[],
                   const MPI_Offset count[], const char *op);

int ncmpi_put_vara_schar(int ncid, int varid, const MPI_Offset start[],
                   const MPI_Offset count[], const signed char *op);

int ncmpi_put_vara_schar_all(int ncid, int varid, const MPI_Offset start[],
                   const MPI_Offset count[], const signed char *op);

int ncmpi_put_vara_short(int ncid, int varid, const MPI_Offset start[],
                   const MPI_Offset count[], const short *op);

int ncmpi_put_vara_short_all(int ncid, int varid, const MPI_Offset start[],
                   const MPI_Offset count[], const short *op);

int ncmpi_put_vara_int(int ncid, int varid, const MPI_Offset start[],
                   const MPI_Offset count[], const int *op);

int ncmpi_put_vara_int_all(int ncid, int varid, const MPI_Offset start[],
                   const MPI_Offset count[], const int *op);

int ncmpi_put_vara_float(int ncid, int varid, const MPI_Offset start[],
                   const MPI_Offset count[], const float *op); 

int ncmpi_put_vara_float_all(int ncid, int varid, const MPI_Offset start[],
                   const MPI_Offset count[], const float *op); 

int ncmpi_put_vara_double(int ncid, int varid, const MPI_Offset start[],
                   const MPI_Offset count[], const double *op); 

int ncmpi_put_vara_double_all(int ncid, int varid, const MPI_Offset start[],
                   const MPI_Offset count[], const double *op); 

int ncmpi_put_vara_longlong(int ncid, int varid, const MPI_Offset start[],
                   const MPI_Offset count[], const long long *op); 

int ncmpi_put_vara_longlong_all(int ncid, int varid, const MPI_Offset start[],
                   const MPI_Offset count[], const long long *op); 

int ncmpi_get_vara(int ncid, int varid, const MPI_Offset start[],
                   const MPI_Offset count[], void *buf, MPI_Offset bufcount,
                   MPI_Datatype buftype);

int ncmpi_get_vara_all(int ncid, int varid, const MPI_Offset start[],
                   const MPI_Offset count[], void *buf, MPI_Offset bufcount,
                   MPI_Datatype buftype);

int ncmpi_get_vara_text(int ncid, int varid, const MPI_Offset start[],
                   const MPI_Offset count[], char *ip); 

int ncmpi_get_vara_text_all(int ncid, int varid, const MPI_Offset start[],
                   const MPI_Offset count[], char *ip); 

int ncmpi_get_vara_schar(int ncid, int varid, const MPI_Offset start[],
                   const MPI_Offset count[], signed char *ip);

int ncmpi_get_vara_schar_all(int ncid, int varid, const MPI_Offset start[],
                   const MPI_Offset count[], signed char *ip);

int ncmpi_get_vara_short(int ncid, int varid, const MPI_Offset start[],
                   const MPI_Offset count[], short *ip); 

int ncmpi_get_vara_short_all(int ncid, int varid, const MPI_Offset start[],
                   const MPI_Offset count[], short *ip); 

int ncmpi_get_vara_int(int ncid, int varid, const MPI_Offset start[],
                   const MPI_Offset count[], int *ip); 

int ncmpi_get_vara_int_all(int ncid, int varid, const MPI_Offset start[],
                   const MPI_Offset count[], int *ip); 

int ncmpi_get_vara_float(int ncid, int varid, const MPI_Offset start[],
                   const MPI_Offset count[], float *ip); 

int ncmpi_get_vara_float_all(int ncid, int varid, const MPI_Offset start[],
                   const MPI_Offset count[], float *ip);

int ncmpi_get_vara_double(int ncid, int varid, const MPI_Offset start[],
                   const MPI_Offset count[], double *ip);

int ncmpi_get_vara_double_all(int ncid, int varid, const MPI_Offset start[],
                   const MPI_Offset count[], double *ip); 

int ncmpi_get_vara_longlong(int ncid, int varid, const MPI_Offset start[],
                   const MPI_Offset count[], long long *ip);

int ncmpi_get_vara_longlong_all(int ncid, int varid, const MPI_Offset start[],
                   const MPI_Offset count[], long long *ip); 

/* Begin Skip Prototypes for Fortran binding */

int ncmpi_put_vara_uchar(int ncid, int varid, const MPI_Offset start[],
                   const MPI_Offset count[], const unsigned char *op);

int ncmpi_put_vara_uchar_all(int ncid, int varid, const MPI_Offset start[],
                   const MPI_Offset count[], const unsigned char *op);

int ncmpi_put_vara_ushort(int ncid, int varid, const MPI_Offset start[],
                   const MPI_Offset count[], const unsigned short *op);

int ncmpi_put_vara_ushort_all(int ncid, int varid, const MPI_Offset start[],
                   const MPI_Offset count[], const unsigned short *op);

int ncmpi_put_vara_uint(int ncid, int varid, const MPI_Offset start[],
                   const MPI_Offset count[], const unsigned int *op);

int ncmpi_put_vara_uint_all(int ncid, int varid, const MPI_Offset start[],
                   const MPI_Offset count[], const unsigned int *op);

int ncmpi_put_vara_long(int ncid, int varid, const MPI_Offset start[],
                   const MPI_Offset count[], const long *op);

int ncmpi_put_vara_long_all(int ncid, int varid, const MPI_Offset start[],
                   const MPI_Offset count[], const long *op);

int ncmpi_put_vara_ulonglong(int ncid, int varid, const MPI_Offset start[],
                   const MPI_Offset count[], const unsigned long long *op);

int ncmpi_put_vara_ulonglong_all(int ncid, int varid, const MPI_Offset start[],
                   const MPI_Offset count[], const unsigned long long *op);

int ncmpi_get_vara_uchar(int ncid, int varid, const MPI_Offset start[],
                   const MPI_Offset count[], unsigned char *ip);

int ncmpi_get_vara_uchar_all(int ncid, int varid, const MPI_Offset start[],
                   const MPI_Offset count[], unsigned char *ip); 

int ncmpi_get_vara_ushort(int ncid, int varid, const MPI_Offset start[],
                   const MPI_Offset count[], unsigned short *ip);

int ncmpi_get_vara_ushort_all(int ncid, int varid, const MPI_Offset start[],
                   const MPI_Offset count[], unsigned short *ip); 

int ncmpi_get_vara_uint(int ncid, int varid, const MPI_Offset start[],
                   const MPI_Offset count[], unsigned int *ip);

int ncmpi_get_vara_uint_all(int ncid, int varid, const MPI_Offset start[],
                   const MPI_Offset count[], unsigned int *ip); 

int ncmpi_get_vara_long(int ncid, int varid, const MPI_Offset start[],
                   const MPI_Offset count[], long *ip);

int ncmpi_get_vara_long_all(int ncid, int varid, const MPI_Offset start[],
                   const MPI_Offset count[], long *ip); 

int ncmpi_get_vara_ulonglong(int ncid, int varid, const MPI_Offset start[],
                   const MPI_Offset count[], unsigned long long *ip);

int ncmpi_get_vara_ulonglong_all(int ncid, int varid, const MPI_Offset start[],
                   const MPI_Offset count[], unsigned long long *ip); 

/* End Skip Prototypes for Fortran binding */

/* End {put,get}_vara */

/* Begin {put,get}_vars */

int ncmpi_put_vars(int ncid, int varid, const MPI_Offset start[],
                   const MPI_Offset count[], const MPI_Offset stride[],
                   const void *buf, MPI_Offset bufcount,
                   MPI_Datatype buftype); 

int ncmpi_put_vars_all(int ncid, int varid, const MPI_Offset start[],
                   const MPI_Offset count[], const MPI_Offset stride[],
                   const void *buf, MPI_Offset bufcount,
                   MPI_Datatype buftype); 

int ncmpi_put_vars_text(int ncid, int varid, const MPI_Offset start[],
                   const MPI_Offset count[], const MPI_Offset stride[],
                   const char *op); 

int ncmpi_put_vars_text_all(int ncid, int varid, const MPI_Offset start[],
                   const MPI_Offset count[], const MPI_Offset stride[],
                   const char *op); 

int ncmpi_put_vars_schar(int ncid, int varid, const MPI_Offset start[],
                   const MPI_Offset count[], const MPI_Offset stride[],
                   const signed char *op); 

int ncmpi_put_vars_schar_all(int ncid, int varid, const MPI_Offset start[],
                   const MPI_Offset count[], const MPI_Offset stride[],
                   const signed char *op); 

int ncmpi_put_vars_short(int ncid, int varid, const MPI_Offset start[],
                   const MPI_Offset count[], const MPI_Offset stride[],
                   const short *op); 

int ncmpi_put_vars_short_all(int ncid, int varid, const MPI_Offset start[],
                   const MPI_Offset count[], const MPI_Offset stride[],
                   const short *op); 

int ncmpi_put_vars_int(int ncid, int varid, const MPI_Offset start[],
                   const MPI_Offset count[], const MPI_Offset stride[],
                   const int *op); 

int ncmpi_put_vars_int_all(int ncid, int varid, const MPI_Offset start[],
                   const MPI_Offset count[], const MPI_Offset stride[],
                   const int *op); 

int ncmpi_put_vars_float(int ncid, int varid, const MPI_Offset start[],
                   const MPI_Offset count[], const MPI_Offset stride[],
                   const float *op); 

int ncmpi_put_vars_float_all(int ncid, int varid, const MPI_Offset start[],
                   const MPI_Offset count[], const MPI_Offset stride[],
                   const float *op);

int ncmpi_put_vars_double(int ncid, int varid, const MPI_Offset start[],
                   const MPI_Offset count[], const MPI_Offset stride[],
                   const double *op); 

int ncmpi_put_vars_double_all(int ncid, int varid, const MPI_Offset start[],
                   const MPI_Offset count[], const MPI_Offset stride[],
                   const double *op); 

int ncmpi_put_vars_longlong(int ncid, int varid, const MPI_Offset start[],
                   const MPI_Offset count[], const MPI_Offset stride[],
                   const long long *op); 

int ncmpi_put_vars_longlong_all(int ncid, int varid, const MPI_Offset start[],
                   const MPI_Offset count[], const MPI_Offset stride[],
                   const long long *op); 

int ncmpi_get_vars(int ncid, int varid, const MPI_Offset start[],
                   const MPI_Offset count[], const MPI_Offset stride[],
                   void *buf, MPI_Offset bufcount, MPI_Datatype buftype); 

int ncmpi_get_vars_all(int ncid, int varid, const MPI_Offset start[],
                   const MPI_Offset count[], const MPI_Offset stride[],
                   void *buf, MPI_Offset bufcount, MPI_Datatype buftype); 

int ncmpi_get_vars_schar(int ncid, int varid, const MPI_Offset start[],
                   const MPI_Offset count[], const MPI_Offset stride[],
                   signed char *ip); 

int ncmpi_get_vars_schar_all(int ncid, int varid, const MPI_Offset start[],
                   const MPI_Offset count[], const MPI_Offset stride[],
                   signed char *ip); 

int ncmpi_get_vars_text(int ncid, int varid, const MPI_Offset start[],
                   const MPI_Offset count[], const MPI_Offset stride[],
                   char *ip); 

int ncmpi_get_vars_text_all(int ncid, int varid, const MPI_Offset start[],
                   const MPI_Offset count[], const MPI_Offset stride[],
                   char *ip); 

int ncmpi_get_vars_short(int ncid, int varid, const MPI_Offset start[],
                   const MPI_Offset count[], const MPI_Offset stride[],
                   short *ip); 

int ncmpi_get_vars_short_all(int ncid, int varid, const MPI_Offset start[],
                   const MPI_Offset count[], const MPI_Offset stride[],
                   short *ip); 

int ncmpi_get_vars_int(int ncid, int varid, const MPI_Offset start[],
                   const MPI_Offset count[], const MPI_Offset stride[],
                   int *ip); 

int ncmpi_get_vars_int_all(int ncid, int varid, const MPI_Offset start[],
                   const MPI_Offset count[], const MPI_Offset stride[],
                   int *ip); 

int ncmpi_get_vars_float(int ncid, int varid, const MPI_Offset start[],
                   const MPI_Offset count[], const MPI_Offset stride[],
                   float *ip); 

int ncmpi_get_vars_float_all(int ncid, int varid, const MPI_Offset start[],
                   const MPI_Offset count[], const MPI_Offset stride[],
                   float *ip); 

int ncmpi_get_vars_double(int ncid, int varid, const MPI_Offset start[],
                   const MPI_Offset count[], const MPI_Offset stride[],
                   double *ip);

int ncmpi_get_vars_double_all(int ncid, int varid, const MPI_Offset start[],
                   const MPI_Offset count[], const MPI_Offset stride[],
                   double *ip); 

int ncmpi_get_vars_longlong(int ncid, int varid, const MPI_Offset start[],
                   const MPI_Offset count[], const MPI_Offset stride[],
                   long long *ip);

int ncmpi_get_vars_longlong_all(int ncid, int varid, const MPI_Offset start[],
                   const MPI_Offset count[], const MPI_Offset stride[],
                   long long *ip); 

/* Begin Skip Prototypes for Fortran binding */

int ncmpi_put_vars_uchar(int ncid, int varid, const MPI_Offset start[],
                   const MPI_Offset count[], const MPI_Offset stride[],
                   const unsigned char *op);

int ncmpi_put_vars_uchar_all(int ncid, int varid, const MPI_Offset start[],
                   const MPI_Offset count[], const MPI_Offset stride[],
                   const unsigned char *op); 

int ncmpi_put_vars_ushort(int ncid, int varid, const MPI_Offset start[],
                   const MPI_Offset count[], const MPI_Offset stride[],
                   const unsigned short *op);

int ncmpi_put_vars_ushort_all(int ncid, int varid, const MPI_Offset start[],
                   const MPI_Offset count[], const MPI_Offset stride[],
                   const unsigned short *op); 

int ncmpi_put_vars_uint(int ncid, int varid, const MPI_Offset start[],
                   const MPI_Offset count[], const MPI_Offset stride[],
                   const unsigned int *op);

int ncmpi_put_vars_uint_all(int ncid, int varid, const MPI_Offset start[],
                   const MPI_Offset count[], const MPI_Offset stride[],
                   const unsigned int *op); 

int ncmpi_put_vars_long(int ncid, int varid, const MPI_Offset start[],
                   const MPI_Offset count[], const MPI_Offset stride[],
                   const long *op);

int ncmpi_put_vars_long_all(int ncid, int varid, const MPI_Offset start[],
                   const MPI_Offset count[], const MPI_Offset stride[],
                   const long *op); 

int ncmpi_put_vars_ulonglong(int ncid, int varid, const MPI_Offset start[],
                   const MPI_Offset count[], const MPI_Offset stride[],
                   const unsigned long long *op);

int ncmpi_put_vars_ulonglong_all(int ncid, int varid, const MPI_Offset start[],
                   const MPI_Offset count[], const MPI_Offset stride[],
                   const unsigned long long *op); 

int ncmpi_get_vars_uchar(int ncid, int varid, const MPI_Offset start[],
                   const MPI_Offset count[], const MPI_Offset stride[],
                   unsigned char *ip);

int ncmpi_get_vars_uchar_all(int ncid, int varid, const MPI_Offset start[],
                   const MPI_Offset count[], const MPI_Offset stride[],
                   unsigned char *ip); 

int ncmpi_get_vars_ushort(int ncid, int varid, const MPI_Offset start[],
                   const MPI_Offset count[], const MPI_Offset stride[],
                   unsigned short *ip);

int ncmpi_get_vars_ushort_all(int ncid, int varid, const MPI_Offset start[],
                   const MPI_Offset count[], const MPI_Offset stride[],
                   unsigned short *ip); 

int ncmpi_get_vars_uint(int ncid, int varid, const MPI_Offset start[],
                   const MPI_Offset count[], const MPI_Offset stride[],
                   unsigned int *ip);

int ncmpi_get_vars_uint_all(int ncid, int varid, const MPI_Offset start[],
                   const MPI_Offset count[], const MPI_Offset stride[],
                   unsigned int *ip); 

int ncmpi_get_vars_long(int ncid, int varid, const MPI_Offset start[],
                   const MPI_Offset count[], const MPI_Offset stride[],
                   long *ip);

int ncmpi_get_vars_long_all(int ncid, int varid, const MPI_Offset start[],
                   const MPI_Offset count[], const MPI_Offset stride[],
                   long *ip); 

int ncmpi_get_vars_ulonglong(int ncid, int varid, const MPI_Offset start[],
                   const MPI_Offset count[], const MPI_Offset stride[],
                   unsigned long long *ip);

int ncmpi_get_vars_ulonglong_all(int ncid, int varid, const MPI_Offset start[],
                   const MPI_Offset count[], const MPI_Offset stride[],
                   unsigned long long *ip); 

/* End Skip Prototypes for Fortran binding */

/* End {put,get}_vars */

/* Begin {put,get}_varm */

int ncmpi_put_varm(int ncid, int varid, const MPI_Offset start[],
                   const MPI_Offset count[], const MPI_Offset stride[],
                   const MPI_Offset imap[], const void *buf,
                   MPI_Offset bufcount, MPI_Datatype buftype); 

int ncmpi_put_varm_all(int ncid, int varid, const MPI_Offset start[],
                   const MPI_Offset count[], const MPI_Offset stride[],
                   const MPI_Offset imap[], const void *buf,
                   MPI_Offset bufcount, MPI_Datatype buftype); 

int ncmpi_put_varm_text(int ncid, int varid, const MPI_Offset start[],
                   const MPI_Offset count[], const MPI_Offset stride[],
                   const MPI_Offset imap[], const char *op); 

int ncmpi_put_varm_text_all(int ncid, int varid, const MPI_Offset start[],
                   const MPI_Offset count[], const MPI_Offset stride[],
                   const MPI_Offset imap[], const char *op); 

int ncmpi_put_varm_schar(int ncid, int varid, const MPI_Offset start[],
                   const MPI_Offset count[], const MPI_Offset stride[],
                   const MPI_Offset imap[], const signed char *op); 

int ncmpi_put_varm_schar_all(int ncid, int varid, const MPI_Offset start[],
                   const MPI_Offset count[], const MPI_Offset stride[],
                   const MPI_Offset imap[], const signed char *op); 

int ncmpi_put_varm_short(int ncid, int varid, const MPI_Offset start[],
                   const MPI_Offset count[], const MPI_Offset stride[],
                   const MPI_Offset imap[], const short *op); 

int ncmpi_put_varm_short_all(int ncid, int varid, const MPI_Offset start[],
                   const MPI_Offset count[], const MPI_Offset stride[],
                   const MPI_Offset imap[], const short *op); 

int ncmpi_put_varm_int(int ncid, int varid, const MPI_Offset start[],
                   const MPI_Offset count[], const MPI_Offset stride[],
                   const MPI_Offset imap[], const int *op);

int ncmpi_put_varm_int_all(int ncid, int varid, const MPI_Offset start[],
                   const MPI_Offset count[], const MPI_Offset stride[],
                   const MPI_Offset imap[], const int *op); 

int ncmpi_put_varm_float(int ncid, int varid, const MPI_Offset start[],
                   const MPI_Offset count[], const MPI_Offset stride[],
                   const MPI_Offset imap[], const float *op); 

int ncmpi_put_varm_float_all(int ncid, int varid, const MPI_Offset start[],
                   const MPI_Offset count[], const MPI_Offset stride[],
                   const MPI_Offset imap[], const float *op); 

int ncmpi_put_varm_double(int ncid, int varid, const MPI_Offset start[],
                   const MPI_Offset count[], const MPI_Offset stride[],
                   const MPI_Offset imap[], const double *op); 

int ncmpi_put_varm_double_all(int ncid, int varid, const MPI_Offset start[],
                   const MPI_Offset count[], const MPI_Offset stride[],
                   const MPI_Offset imap[], const double *op); 

int ncmpi_put_varm_longlong(int ncid, int varid, const MPI_Offset start[],
                   const MPI_Offset count[], const MPI_Offset stride[],
                   const MPI_Offset imap[], const long long *op); 

int ncmpi_put_varm_longlong_all(int ncid, int varid, const MPI_Offset start[],
                   const MPI_Offset count[], const MPI_Offset stride[],
                   const MPI_Offset imap[], const long long *op); 

int ncmpi_get_varm(int ncid, int varid, const MPI_Offset start[],
                   const MPI_Offset count[], const MPI_Offset stride[],
                   const MPI_Offset imap[], void *buf, MPI_Offset bufcount,
                   MPI_Datatype buftype); 

int ncmpi_get_varm_all(int ncid, int varid, const MPI_Offset start[],
                   const MPI_Offset count[], const MPI_Offset stride[],
                   const MPI_Offset imap[], void *buf, MPI_Offset bufcount,
                   MPI_Datatype buftype); 

int ncmpi_get_varm_schar(int ncid, int varid, const MPI_Offset start[],
                   const MPI_Offset count[], const MPI_Offset stride[],
                   const MPI_Offset imap[], signed char *ip); 

int ncmpi_get_varm_schar_all(int ncid, int varid, const MPI_Offset start[],
                   const MPI_Offset count[], const MPI_Offset stride[],
                   const MPI_Offset imap[], signed char *ip); 

int ncmpi_get_varm_text(int ncid, int varid, const MPI_Offset start[],
                   const MPI_Offset count[], const MPI_Offset stride[],
                   const MPI_Offset imap[], char *ip); 

int ncmpi_get_varm_text_all(int ncid, int varid, const MPI_Offset start[],
                   const MPI_Offset count[], const MPI_Offset stride[],
                   const MPI_Offset imap[], char *ip); 

int ncmpi_get_varm_short(int ncid, int varid, const MPI_Offset start[],
                   const MPI_Offset count[], const MPI_Offset stride[],
                   const MPI_Offset imap[], short *ip); 

int ncmpi_get_varm_short_all(int ncid, int varid, const MPI_Offset start[],
                   const MPI_Offset count[], const MPI_Offset stride[],
                   const MPI_Offset imap[], short *ip); 

int ncmpi_get_varm_int(int ncid, int varid, const MPI_Offset start[],
                   const MPI_Offset count[], const MPI_Offset stride[],
                   const MPI_Offset imap[], int *ip); 

int ncmpi_get_varm_int_all(int ncid, int varid, const MPI_Offset start[],
                   const MPI_Offset count[], const MPI_Offset stride[],
                   const MPI_Offset imap[], int *ip); 

int ncmpi_get_varm_float(int ncid, int varid, const MPI_Offset start[],
                   const MPI_Offset count[], const MPI_Offset stride[],
                   const MPI_Offset imap[], float *ip); 

int ncmpi_get_varm_float_all(int ncid, int varid, const MPI_Offset start[],
                   const MPI_Offset count[], const MPI_Offset stride[],
                   const MPI_Offset imap[], float *ip); 

int ncmpi_get_varm_double(int ncid, int varid, const MPI_Offset start[],
                   const MPI_Offset count[], const MPI_Offset stride[],
                   const MPI_Offset imap[], double *ip);

int ncmpi_get_varm_double_all(int ncid, int varid, const MPI_Offset start[],
                   const MPI_Offset count[], const MPI_Offset stride[],
                   const MPI_Offset imap[], double *ip); 

int ncmpi_get_varm_longlong(int ncid, int varid, const MPI_Offset start[],
                   const MPI_Offset count[], const MPI_Offset stride[],
                   const MPI_Offset imap[], long long *ip);

int ncmpi_get_varm_longlong_all(int ncid, int varid, const MPI_Offset start[],
                   const MPI_Offset count[], const MPI_Offset stride[],
                   const MPI_Offset imap[], long long *ip); 

/* Begin Skip Prototypes for Fortran binding */ 

int ncmpi_put_varm_uchar(int ncid, int varid, const MPI_Offset start[],
                   const MPI_Offset count[], const MPI_Offset stride[],
                   const MPI_Offset imap[], const unsigned char *op);

int ncmpi_put_varm_uchar_all(int ncid, int varid, const MPI_Offset start[],
                   const MPI_Offset count[], const MPI_Offset stride[],
                   const MPI_Offset imap[], const unsigned char *op); 

int ncmpi_put_varm_ushort(int ncid, int varid, const MPI_Offset start[],
                   const MPI_Offset count[], const MPI_Offset stride[],
                   const MPI_Offset imap[], const unsigned short *op);

int ncmpi_put_varm_ushort_all(int ncid, int varid, const MPI_Offset start[],
                   const MPI_Offset count[], const MPI_Offset stride[],
                   const MPI_Offset imap[], const unsigned short *op); 

int ncmpi_put_varm_uint(int ncid, int varid, const MPI_Offset start[],
                   const MPI_Offset count[], const MPI_Offset stride[],
                   const MPI_Offset imap[], const unsigned int *op);

int ncmpi_put_varm_uint_all(int ncid, int varid, const MPI_Offset start[],
                   const MPI_Offset count[], const MPI_Offset stride[],
                   const MPI_Offset imap[], const unsigned int *op); 

int ncmpi_put_varm_long(int ncid, int varid, const MPI_Offset start[],
                   const MPI_Offset count[], const MPI_Offset stride[],
                   const MPI_Offset imap[], const long *op);

int ncmpi_put_varm_long_all(int ncid, int varid, const MPI_Offset start[],
                   const MPI_Offset count[], const MPI_Offset stride[],
                   const MPI_Offset imap[], const long *op); 

int ncmpi_put_varm_ulonglong(int ncid, int varid, const MPI_Offset start[],
                   const MPI_Offset count[], const MPI_Offset stride[],
                   const MPI_Offset imap[], const unsigned long long *op);

int ncmpi_put_varm_ulonglong_all(int ncid, int varid, const MPI_Offset start[],
                   const MPI_Offset count[], const MPI_Offset stride[],
                   const MPI_Offset imap[], const unsigned long long *op); 

int ncmpi_get_varm_uchar(int ncid, int varid, const MPI_Offset start[],
                   const MPI_Offset count[], const MPI_Offset stride[],
                   const MPI_Offset imap[], unsigned char *ip); 

int ncmpi_get_varm_uchar_all(int ncid, int varid, const MPI_Offset start[],
                   const MPI_Offset count[], const MPI_Offset stride[],
                   const MPI_Offset imap[], unsigned char *ip); 

int ncmpi_get_varm_ushort(int ncid, int varid, const MPI_Offset start[],
                   const MPI_Offset count[], const MPI_Offset stride[],
                   const MPI_Offset imap[], unsigned short *ip); 

int ncmpi_get_varm_ushort_all(int ncid, int varid, const MPI_Offset start[],
                   const MPI_Offset count[], const MPI_Offset stride[],
                   const MPI_Offset imap[], unsigned short *ip); 

int ncmpi_get_varm_uint(int ncid, int varid, const MPI_Offset start[],
                   const MPI_Offset count[], const MPI_Offset stride[],
                   const MPI_Offset imap[], unsigned int *ip); 

int ncmpi_get_varm_uint_all(int ncid, int varid, const MPI_Offset start[],
                   const MPI_Offset count[], const MPI_Offset stride[],
                   const MPI_Offset imap[], unsigned int *ip); 

int ncmpi_get_varm_long(int ncid, int varid, const MPI_Offset start[],
                   const MPI_Offset count[], const MPI_Offset stride[],
                   const MPI_Offset imap[], long *ip);

int ncmpi_get_varm_long_all(int ncid, int varid, const MPI_Offset start[],
                   const MPI_Offset count[], const MPI_Offset stride[],
                   const MPI_Offset imap[], long *ip); 

int ncmpi_get_varm_ulonglong(int ncid, int varid, const MPI_Offset start[],
                   const MPI_Offset count[], const MPI_Offset stride[],
                   const MPI_Offset imap[], unsigned long long *ip); 

int ncmpi_get_varm_ulonglong_all(int ncid, int varid, const MPI_Offset start[],
                   const MPI_Offset count[], const MPI_Offset stride[],
                   const MPI_Offset imap[], unsigned long long *ip); 

/* End Skip Prototypes for Fortran binding */ 

/* End {put,get}_varm */

/* #################################################################### */
/* Begin: more prototypes to be included for fortran binding conversion */

/* Begin non-blocking data access functions */

int ncmpi_wait(int ncid, int count, int array_of_requests[],
                   int array_of_statuses[]);

int ncmpi_wait_all(int ncid, int count, int array_of_requests[],
                   int array_of_statuses[]);

int ncmpi_cancel(int ncid, int num, int *requests, int *statuses);

/* Begin {iput,iget,bput}_var1 */

int ncmpi_iput_var1(int ncid, int varid, const MPI_Offset index[],
                    const void *buf, MPI_Offset bufcount,
                    MPI_Datatype buftype, int *request); 

int ncmpi_iput_var1_text(int ncid, int varid, const MPI_Offset index[],
                    const char *op, int *request); 

int ncmpi_iput_var1_schar(int ncid, int varid, const MPI_Offset index[],
                    const signed char *op, int *request); 

int ncmpi_iput_var1_short(int ncid, int varid, const MPI_Offset index[],
                    const short *op, int *request); 

int ncmpi_iput_var1_int(int ncid, int varid, const MPI_Offset index[],
                    const int *op, int *request); 

int ncmpi_iput_var1_float(int ncid, int varid, const MPI_Offset index[],
                    const float *op, int *request); 

int ncmpi_iput_var1_double(int ncid, int varid, const MPI_Offset index[],
                    const double *op, int *request); 

int ncmpi_iput_var1_longlong(int ncid, int varid, const MPI_Offset index[],
                    const long long *op, int *request); 

int ncmpi_iget_var1(int ncid, int varid, const MPI_Offset index[], void *buf,
                    MPI_Offset bufcount, MPI_Datatype buftype, int *request); 

int ncmpi_iget_var1_schar(int ncid, int varid, const MPI_Offset index[],
                    signed char *ip, int *request); 

int ncmpi_iget_var1_text(int ncid, int varid, const MPI_Offset index[],
                    char *ip, int *request); 

int ncmpi_iget_var1_short(int ncid, int varid, const MPI_Offset index[],
                    short *ip, int *request); 

int ncmpi_iget_var1_int(int ncid, int varid, const MPI_Offset index[],
                    int *ip, int *request); 

int ncmpi_iget_var1_float(int ncid, int varid, const MPI_Offset index[],
                    float *ip, int *request); 

int ncmpi_iget_var1_double(int ncid, int varid, const MPI_Offset index[],
                    double *ip, int *request);

int ncmpi_iget_var1_longlong(int ncid, int varid, const MPI_Offset index[],
                    long long *ip, int *request);

int ncmpi_bput_var1(int ncid, int varid, const MPI_Offset index[],
                    const void *buf, MPI_Offset bufcount,
                    MPI_Datatype buftype, int *request); 

int ncmpi_bput_var1_text(int ncid, int varid, const MPI_Offset index[],
                    const char *op, int *request); 

int ncmpi_bput_var1_schar(int ncid, int varid, const MPI_Offset index[],
                    const signed char *op, int *request); 

int ncmpi_bput_var1_short(int ncid, int varid, const MPI_Offset index[],
                    const short *op, int *request); 

int ncmpi_bput_var1_int(int ncid, int varid, const MPI_Offset index[],
                    const int *op, int *request); 

int ncmpi_bput_var1_float(int ncid, int varid, const MPI_Offset index[],
                    const float *op, int *request); 

int ncmpi_bput_var1_double(int ncid, int varid, const MPI_Offset index[],
                    const double *op, int *request); 

int ncmpi_bput_var1_longlong(int ncid, int varid, const MPI_Offset index[],
                    const long long *op, int *request); 

/* Begin Skip Prototypes for Fortran binding */

int ncmpi_iput_var1_uchar(int ncid, int varid, const MPI_Offset index[],
                    const unsigned char *op, int *request);

int ncmpi_iput_var1_ushort(int ncid, int varid, const MPI_Offset index[],
                    const unsigned short *op, int *request);

int ncmpi_iput_var1_uint(int ncid, int varid, const MPI_Offset index[],
                    const unsigned int *op, int *request);

int ncmpi_iput_var1_long(int ncid, int varid, const MPI_Offset index[],
                    const long *ip, int *request);

int ncmpi_iput_var1_ulonglong(int ncid, int varid, const MPI_Offset index[],
                    const unsigned long long *op, int *request);

int ncmpi_iget_var1_uchar(int ncid, int varid, const MPI_Offset index[],
                    unsigned char *ip, int *request);

int ncmpi_iget_var1_ushort(int ncid, int varid, const MPI_Offset index[],
                    unsigned short *ip, int *request);

int ncmpi_iget_var1_uint(int ncid, int varid, const MPI_Offset index[],
                    unsigned int *ip, int *request);

int ncmpi_iget_var1_long(int ncid, int varid, const MPI_Offset index[],
                    long *ip, int *request);

int ncmpi_iget_var1_ulonglong(int ncid, int varid, const MPI_Offset index[],
                    unsigned long long *ip, int *request);

int ncmpi_bput_var1_uchar(int ncid, int varid, const MPI_Offset index[],
                    const unsigned char *op, int *request);

int ncmpi_bput_var1_ushort(int ncid, int varid, const MPI_Offset index[],
                    const unsigned short *op, int *request);

int ncmpi_bput_var1_uint(int ncid, int varid, const MPI_Offset index[],
                    const unsigned int *op, int *request);

int ncmpi_bput_var1_long(int ncid, int varid, const MPI_Offset index[],
                    const long *ip, int *request);

int ncmpi_bput_var1_ulonglong(int ncid, int varid, const MPI_Offset index[],
                    const unsigned long long *op, int *request);

/* End Skip Prototypes for Fortran binding */

/* End {iput,iget,bput}_var1 */

/* Begin {iput,iget,bput}_var */  

int ncmpi_iput_var(int ncid, int varid, const void *buf, MPI_Offset bufcount,
                    MPI_Datatype buftype, int *request);

int ncmpi_iput_var_schar(int ncid, int varid, const signed char *op,
                    int *request);

int ncmpi_iput_var_text(int ncid, int varid, const char *op, int *request);

int ncmpi_iput_var_short(int ncid, int varid, const short *op, int *request);

int ncmpi_iput_var_int(int ncid, int varid, const int *op, int *request);

int ncmpi_iput_var_float(int ncid, int varid, const float *op, int *request);

int ncmpi_iput_var_double(int ncid, int varid, const double *op, int *request);

int ncmpi_iput_var_longlong(int ncid, int varid, const long long *op, int *request);

int ncmpi_iget_var(int ncid, int varid, void *buf, MPI_Offset bufcount,
                    MPI_Datatype buftype, int *request);

int ncmpi_iget_var_schar(int ncid, int varid, signed char *ip, int *request);

int ncmpi_iget_var_text(int ncid, int varid, char *ip, int *request);

int ncmpi_iget_var_short(int ncid, int varid, short *ip, int *request);

int ncmpi_iget_var_int(int ncid, int varid, int *ip, int *request);

int ncmpi_iget_var_float(int ncid, int varid, float *ip, int *request);

int ncmpi_iget_var_double(int ncid, int varid, double *ip, int *request);

int ncmpi_iget_var_longlong(int ncid, int varid, long long *ip, int *request);

int ncmpi_bput_var(int ncid, int varid, const void *buf, MPI_Offset bufcount,
                    MPI_Datatype buftype, int *request);

int ncmpi_bput_var_schar(int ncid, int varid, const signed char *op,
                    int *request);

int ncmpi_bput_var_text(int ncid, int varid, const char *op, int *request);

int ncmpi_bput_var_short(int ncid, int varid, const short *op, int *request);

int ncmpi_bput_var_int(int ncid, int varid, const int *op, int *request);

int ncmpi_bput_var_float(int ncid, int varid, const float *op, int *request);

int ncmpi_bput_var_double(int ncid, int varid, const double *op, int *request);

int ncmpi_bput_var_longlong(int ncid, int varid, const long long *op, int *request);

/* Begin Skip Prototypes for Fortran binding */

int ncmpi_iput_var_uchar(int ncid, int varid, const unsigned char *op,
                    int *request);

int ncmpi_iput_var_ushort(int ncid, int varid, const unsigned short *op,
                    int *request);

int ncmpi_iput_var_uint(int ncid, int varid, const unsigned int *op,
                    int *request);

int ncmpi_iput_var_long(int ncid, int varid, const long *op, int *request);

int ncmpi_iput_var_ulonglong(int ncid, int varid, const unsigned long long *op,
                    int *request);

int ncmpi_iget_var_uchar(int ncid, int varid, unsigned char *ip, int *request);

int ncmpi_iget_var_ushort(int ncid, int varid, unsigned short *ip, int *request);

int ncmpi_iget_var_uint(int ncid, int varid, unsigned int *ip, int *request);

int ncmpi_iget_var_long(int ncid, int varid, long *ip, int *request);

int ncmpi_iget_var_ulonglong(int ncid, int varid, unsigned long long *ip, int *request);

int ncmpi_bput_var_uchar(int ncid, int varid, const unsigned char *op,
                    int *request);

int ncmpi_bput_var_ushort(int ncid, int varid, const unsigned short *op,
                    int *request);

int ncmpi_bput_var_uint(int ncid, int varid, const unsigned int *op,
                    int *request);

int ncmpi_bput_var_long(int ncid, int varid, const long *op, int *request);

int ncmpi_bput_var_ulonglong(int ncid, int varid, const unsigned long long *op,
                    int *request);

/* End Skip Prototypes for Fortran binding */

/* End {iput,iget,bput}_var */

/* Begin {iput,iget,bput}_vara */

int ncmpi_iput_vara(int ncid, int varid, const MPI_Offset start[],
                    const MPI_Offset count[], const void *buf,
                    MPI_Offset bufcount, MPI_Datatype buftype, int *request); 

int ncmpi_iput_vara_schar(int ncid, int varid, const MPI_Offset start[],
                    const MPI_Offset count[], const signed char *op,
                    int *request); 

int ncmpi_iput_vara_text(int ncid, int varid, const MPI_Offset start[],
                    const MPI_Offset count[], const char *op, int *request); 

int ncmpi_iput_vara_short(int ncid, int varid, const MPI_Offset start[],
                    const MPI_Offset count[], const short *op, int *request); 

int ncmpi_iput_vara_int(int ncid, int varid, const MPI_Offset start[],
                    const MPI_Offset count[], const int *op, int *request); 

int ncmpi_iput_vara_float(int ncid, int varid, const MPI_Offset start[],
                    const MPI_Offset count[], const float *op, int *request); 

int ncmpi_iput_vara_double(int ncid, int varid, const MPI_Offset start[],
                    const MPI_Offset count[], const double *op, int *request); 

int ncmpi_iput_vara_longlong(int ncid, int varid, const MPI_Offset start[],
                    const MPI_Offset count[], const long long *op, int *request); 

int ncmpi_iget_vara(int ncid, int varid, const MPI_Offset start[],
                    const MPI_Offset count[], void *buf, MPI_Offset bufcount,
                    MPI_Datatype buftype, int *request); 

int ncmpi_iget_vara_schar(int ncid, int varid, const MPI_Offset start[],
                    const MPI_Offset count[], signed char *ip, int *request); 

int ncmpi_iget_vara_text(int ncid, int varid, const MPI_Offset start[],
                    const MPI_Offset count[], char *ip, int *request); 

int ncmpi_iget_vara_short(int ncid, int varid, const MPI_Offset start[],
                    const MPI_Offset count[], short *ip, int *request); 

int ncmpi_iget_vara_int(int ncid, int varid, const MPI_Offset start[],
                    const MPI_Offset count[], int *ip, int *request); 

int ncmpi_iget_vara_float(int ncid, int varid, const MPI_Offset start[],
                    const MPI_Offset count[], float *ip, int *request); 

int ncmpi_iget_vara_double(int ncid, int varid, const MPI_Offset start[],
                    const MPI_Offset count[], double *ip, int *request);

int ncmpi_iget_vara_longlong(int ncid, int varid, const MPI_Offset start[],
                    const MPI_Offset count[], long long *ip, int *request);

int ncmpi_bput_vara(int ncid, int varid, const MPI_Offset start[],
                    const MPI_Offset count[], const void *buf,
                    MPI_Offset bufcount, MPI_Datatype buftype, int *request); 

int ncmpi_bput_vara_schar(int ncid, int varid, const MPI_Offset start[],
                    const MPI_Offset count[], const signed char *op,
                    int *request); 

int ncmpi_bput_vara_text(int ncid, int varid, const MPI_Offset start[],
                    const MPI_Offset count[], const char *op, int *request); 

int ncmpi_bput_vara_short(int ncid, int varid, const MPI_Offset start[],
                    const MPI_Offset count[], const short *op, int *request); 

int ncmpi_bput_vara_int(int ncid, int varid, const MPI_Offset start[],
                    const MPI_Offset count[], const int *op, int *request); 

int ncmpi_bput_vara_float(int ncid, int varid, const MPI_Offset start[],
                    const MPI_Offset count[], const float *op, int *request); 

int ncmpi_bput_vara_double(int ncid, int varid, const MPI_Offset start[],
                    const MPI_Offset count[], const double *op, int *request); 

int ncmpi_bput_vara_longlong(int ncid, int varid, const MPI_Offset start[],
                    const MPI_Offset count[], const long long *op, int *request); 

/* Begin Skip Prototypes for Fortran binding */

int ncmpi_iput_vara_uchar(int ncid, int varid, const MPI_Offset start[],
                    const MPI_Offset count[], const unsigned char *op,
                    int *request);

int ncmpi_iput_vara_ushort(int ncid, int varid, const MPI_Offset start[],
                    const MPI_Offset count[], const unsigned short *op,
                    int *request);

int ncmpi_iput_vara_uint(int ncid, int varid, const MPI_Offset start[],
                    const MPI_Offset count[], const unsigned int *op,
                    int *request);

int ncmpi_iput_vara_long(int ncid, int varid, const MPI_Offset start[],
                    const MPI_Offset count[], const long *op, int *request);

int ncmpi_iput_vara_ulonglong(int ncid, int varid, const MPI_Offset start[],
                    const MPI_Offset count[], const unsigned long long *op,
                    int *request);

int ncmpi_iget_vara_uchar(int ncid, int varid, const MPI_Offset start[],
                    const MPI_Offset count[], unsigned char *ip, int *request);

int ncmpi_iget_vara_ushort(int ncid, int varid, const MPI_Offset start[],
                    const MPI_Offset count[], unsigned short *ip, int *request);

int ncmpi_iget_vara_uint(int ncid, int varid, const MPI_Offset start[],
                    const MPI_Offset count[], unsigned int *ip, int *request);

int ncmpi_iget_vara_long(int ncid, int varid, const MPI_Offset start[],
                    const MPI_Offset count[], long *ip, int *request);

int ncmpi_iget_vara_ulonglong(int ncid, int varid, const MPI_Offset start[],
                    const MPI_Offset count[], unsigned long long *ip, int *request);

int ncmpi_bput_vara_uchar(int ncid, int varid, const MPI_Offset start[],
                    const MPI_Offset count[], const unsigned char *op,
                    int *request);

int ncmpi_bput_vara_ushort(int ncid, int varid, const MPI_Offset start[],
                    const MPI_Offset count[], const unsigned short *op,
                    int *request);

int ncmpi_bput_vara_uint(int ncid, int varid, const MPI_Offset start[],
                    const MPI_Offset count[], const unsigned int *op,
                    int *request);

int ncmpi_bput_vara_long(int ncid, int varid, const MPI_Offset start[],
                    const MPI_Offset count[], const long *op, int *request);

int ncmpi_bput_vara_ulonglong(int ncid, int varid, const MPI_Offset start[],
                    const MPI_Offset count[], const unsigned long long *op,
                    int *request);

/* End Skip Prototypes for Fortran binding */

/* End {iput,iget,bput}_vara */

/* Begin {iput,iget,bput}_vars */

int ncmpi_iput_vars(int ncid, int varid, const MPI_Offset start[],
                    const MPI_Offset count[], const MPI_Offset stride[],
                    const void *buf, MPI_Offset bufcount,
                    MPI_Datatype buftype, int *request); 

int ncmpi_iput_vars_schar(int ncid, int varid, const MPI_Offset start[],
                    const MPI_Offset count[], const MPI_Offset stride[],
                    const signed char *op, int *request); 

int ncmpi_iput_vars_text(int ncid, int varid, const MPI_Offset start[],
                    const MPI_Offset count[], const MPI_Offset stride[],
                    const char *op, int *request); 

int ncmpi_iput_vars_short(int ncid, int varid, const MPI_Offset start[],
                    const MPI_Offset count[], const MPI_Offset stride[],
                    const short *op, int *request); 

int ncmpi_iput_vars_int(int ncid, int varid, const MPI_Offset start[],
                    const MPI_Offset count[], const MPI_Offset stride[],
                    const int *op, int *request); 

int ncmpi_iput_vars_float(int ncid, int varid, const MPI_Offset start[],
                    const MPI_Offset count[], const MPI_Offset stride[],
                    const float *op, int *request); 

int ncmpi_iput_vars_double(int ncid, int varid, const MPI_Offset start[],
                    const MPI_Offset count[], const MPI_Offset stride[],
                    const double *op, int *request); 

int ncmpi_iput_vars_longlong(int ncid, int varid, const MPI_Offset start[],
                    const MPI_Offset count[], const MPI_Offset stride[],
                    const long long *op, int *request); 

int ncmpi_iget_vars(int ncid, int varid, const MPI_Offset start[],
                    const MPI_Offset count[], const MPI_Offset stride[],
                    void *buf, MPI_Offset bufcount, MPI_Datatype buftype,
                    int *request); 

int ncmpi_iget_vars_schar(int ncid, int varid, const MPI_Offset start[],
                    const MPI_Offset count[], const MPI_Offset stride[],
                    signed char *ip, int *request); 

int ncmpi_iget_vars_text(int ncid, int varid, const MPI_Offset start[],
                    const MPI_Offset count[], const MPI_Offset stride[],
                    char *ip, int *request); 

int ncmpi_iget_vars_short(int ncid, int varid, const MPI_Offset start[],
                    const MPI_Offset count[], const MPI_Offset stride[],
                    short *ip, int *request); 

int ncmpi_iget_vars_int(int ncid, int varid, const MPI_Offset start[],
                    const MPI_Offset count[], const MPI_Offset stride[],
                    int *ip, int *request); 

int ncmpi_iget_vars_float(int ncid, int varid, const MPI_Offset start[],
                    const MPI_Offset count[], const MPI_Offset stride[],
                    float *ip, int *request); 

int ncmpi_iget_vars_double(int ncid, int varid, const MPI_Offset start[],
                    const MPI_Offset count[], const MPI_Offset stride[],
                    double *ip, int *request); 

int ncmpi_iget_vars_longlong(int ncid, int varid, const MPI_Offset start[],
                    const MPI_Offset count[], const MPI_Offset stride[],
                    long long *ip, int *request); 

int ncmpi_bput_vars(int ncid, int varid, const MPI_Offset start[],
                    const MPI_Offset count[], const MPI_Offset stride[],
                    const void *buf, MPI_Offset bufcount,
                    MPI_Datatype buftype, int *request); 

int ncmpi_bput_vars_schar(int ncid, int varid, const MPI_Offset start[],
                    const MPI_Offset count[], const MPI_Offset stride[],
                    const signed char *op, int *request); 

int ncmpi_bput_vars_text(int ncid, int varid, const MPI_Offset start[],
                    const MPI_Offset count[], const MPI_Offset stride[],
                    const char *op, int *request); 

int ncmpi_bput_vars_short(int ncid, int varid, const MPI_Offset start[],
                    const MPI_Offset count[], const MPI_Offset stride[],
                    const short *op, int *request); 

int ncmpi_bput_vars_int(int ncid, int varid, const MPI_Offset start[],
                    const MPI_Offset count[], const MPI_Offset stride[],
                    const int *op, int *request); 

int ncmpi_bput_vars_float(int ncid, int varid, const MPI_Offset start[],
                    const MPI_Offset count[], const MPI_Offset stride[],
                    const float *op, int *request); 

int ncmpi_bput_vars_double(int ncid, int varid, const MPI_Offset start[],
                    const MPI_Offset count[], const MPI_Offset stride[],
                    const double *op, int *request); 

int ncmpi_bput_vars_longlong(int ncid, int varid, const MPI_Offset start[],
                    const MPI_Offset count[], const MPI_Offset stride[],
                    const long long *op, int *request); 

/* Begin Skip Prototypes for Fortran binding */

int ncmpi_iput_vars_uchar(int ncid, int varid, const MPI_Offset start[],
                    const MPI_Offset count[], const MPI_Offset stride[],
                    const unsigned char *op, int *request);

int ncmpi_iput_vars_ushort(int ncid, int varid, const MPI_Offset start[],
                    const MPI_Offset count[], const MPI_Offset stride[],
                    const unsigned short *op, int *request);

int ncmpi_iput_vars_uint(int ncid, int varid, const MPI_Offset start[],
                    const MPI_Offset count[], const MPI_Offset stride[],
                    const unsigned int *op, int *request);

int ncmpi_iput_vars_long(int ncid, int varid, const MPI_Offset start[],
                    const MPI_Offset count[], const MPI_Offset stride[],
                    const long *op, int *request);

int ncmpi_iput_vars_ulonglong(int ncid, int varid, const MPI_Offset start[],
                    const MPI_Offset count[], const MPI_Offset stride[],
                    const unsigned long long *op, int *request);

int ncmpi_iget_vars_uchar(int ncid, int varid, const MPI_Offset start[],
                    const MPI_Offset count[], const MPI_Offset stride[],
                    unsigned char *ip, int *request);

int ncmpi_iget_vars_ushort(int ncid, int varid, const MPI_Offset start[],
                    const MPI_Offset count[], const MPI_Offset stride[],
                    unsigned short *ip, int *request);

int ncmpi_iget_vars_uint(int ncid, int varid, const MPI_Offset start[],
                    const MPI_Offset count[], const MPI_Offset stride[],
                    unsigned int *ip, int *request);

int ncmpi_iget_vars_long(int ncid, int varid, const MPI_Offset start[],
                    const MPI_Offset count[], const MPI_Offset stride[],
                    long *ip, int *request); 

int ncmpi_iget_vars_ulonglong(int ncid, int varid, const MPI_Offset start[],
                    const MPI_Offset count[], const MPI_Offset stride[],
                    unsigned long long *ip, int *request);

int ncmpi_bput_vars_uchar(int ncid, int varid, const MPI_Offset start[],
                    const MPI_Offset count[], const MPI_Offset stride[],
                    const unsigned char *op, int *request);

int ncmpi_bput_vars_ushort(int ncid, int varid, const MPI_Offset start[],
                    const MPI_Offset count[], const MPI_Offset stride[],
                    const unsigned short *op, int *request);

int ncmpi_bput_vars_uint(int ncid, int varid, const MPI_Offset start[],
                    const MPI_Offset count[], const MPI_Offset stride[],
                    const unsigned int *op, int *request);

int ncmpi_bput_vars_long(int ncid, int varid, const MPI_Offset start[],
                    const MPI_Offset count[], const MPI_Offset stride[],
                    const long *op, int *request);

int ncmpi_bput_vars_ulonglong(int ncid, int varid, const MPI_Offset start[],
                    const MPI_Offset count[], const MPI_Offset stride[],
                    const unsigned long long *op, int *request);

/* End Skip Prototypes for Fortran binding */

/* End {iput,iget,bput}_vars */

/* Begin {iput,iget,bput}_varm */

int ncmpi_iput_varm(int ncid, int varid, const MPI_Offset start[],
                    const MPI_Offset count[], const MPI_Offset stride[],
                    const MPI_Offset imap[], const void *buf,
                    MPI_Offset bufcount, MPI_Datatype buftype, int *request); 

int ncmpi_iput_varm_schar(int ncid, int varid, const MPI_Offset start[],
                    const MPI_Offset count[], const MPI_Offset stride[],
                    const MPI_Offset imap[], const signed char *op,
                    int *request); 

int ncmpi_iput_varm_text(int ncid, int varid, const MPI_Offset start[],
                    const MPI_Offset count[], const MPI_Offset stride[],
                    const MPI_Offset imap[], const char *op, int *request); 

int ncmpi_iput_varm_short(int ncid, int varid, const MPI_Offset start[],
                    const MPI_Offset count[], const MPI_Offset stride[],
                    const MPI_Offset imap[], const short *op, int *request); 

int ncmpi_iput_varm_int(int ncid, int varid, const MPI_Offset start[],
                    const MPI_Offset count[], const MPI_Offset stride[],
                    const MPI_Offset imap[], const int *op, int *request); 

int ncmpi_iput_varm_float(int ncid, int varid, const MPI_Offset start[],
                    const MPI_Offset count[], const MPI_Offset stride[],
                    const MPI_Offset imap[], const float *op, int *request); 

int ncmpi_iput_varm_double(int ncid, int varid, const MPI_Offset start[],
                    const MPI_Offset count[], const MPI_Offset stride[],
                    const MPI_Offset imap[], const double *op, int *request); 

int ncmpi_iput_varm_longlong(int ncid, int varid, const MPI_Offset start[],
                    const MPI_Offset count[], const MPI_Offset stride[],
                    const MPI_Offset imap[], const long long *op, int *request); 

int ncmpi_iget_varm(int ncid, int varid, const MPI_Offset start[],
                    const MPI_Offset count[], const MPI_Offset stride[],
                    const MPI_Offset imap[], void *buf, MPI_Offset bufcount,
                    MPI_Datatype buftype, int *request); 

int ncmpi_iget_varm_schar(int ncid, int varid, const MPI_Offset start[],
                    const MPI_Offset count[], const MPI_Offset stride[],
                    const MPI_Offset imap[], signed char *ip, int *request); 

int ncmpi_iget_varm_text(int ncid, int varid, const MPI_Offset start[],
                    const MPI_Offset count[], const MPI_Offset stride[],
                    const MPI_Offset imap[], char *ip, int *request); 

int ncmpi_iget_varm_short(int ncid, int varid, const MPI_Offset start[],
                    const MPI_Offset count[], const MPI_Offset stride[],
                    const MPI_Offset imap[], short *ip, int *request); 

int ncmpi_iget_varm_int(int ncid, int varid, const MPI_Offset start[],
                    const MPI_Offset count[], const MPI_Offset stride[],
                    const MPI_Offset imap[], int *ip, int *request); 

int ncmpi_iget_varm_float(int ncid, int varid, const MPI_Offset start[],
                    const MPI_Offset count[], const MPI_Offset stride[],
                    const MPI_Offset imap[], float *ip, int *request); 

int ncmpi_iget_varm_double(int ncid, int varid, const MPI_Offset start[],
                    const MPI_Offset count[], const MPI_Offset stride[],
                    const MPI_Offset imap[], double *ip, int *request);

int ncmpi_iget_varm_longlong(int ncid, int varid, const MPI_Offset start[],
                    const MPI_Offset count[], const MPI_Offset stride[],
                    const MPI_Offset imap[], long long *ip, int *request);

int ncmpi_bput_varm(int ncid, int varid, const MPI_Offset start[],
                    const MPI_Offset count[], const MPI_Offset stride[],
                    const MPI_Offset imap[], const void *buf,
                    MPI_Offset bufcount, MPI_Datatype buftype, int *request); 

int ncmpi_bput_varm_schar(int ncid, int varid, const MPI_Offset start[],
                    const MPI_Offset count[], const MPI_Offset stride[],
                    const MPI_Offset imap[], const signed char *op,
                    int *request); 

int ncmpi_bput_varm_text(int ncid, int varid, const MPI_Offset start[],
                    const MPI_Offset count[], const MPI_Offset stride[],
                    const MPI_Offset imap[], const char *op, int *request); 

int ncmpi_bput_varm_short(int ncid, int varid, const MPI_Offset start[],
                    const MPI_Offset count[], const MPI_Offset stride[],
                    const MPI_Offset imap[], const short *op, int *request); 

int ncmpi_bput_varm_int(int ncid, int varid, const MPI_Offset start[],
                    const MPI_Offset count[], const MPI_Offset stride[],
                    const MPI_Offset imap[], const int *op, int *request); 

int ncmpi_bput_varm_float(int ncid, int varid, const MPI_Offset start[],
                    const MPI_Offset count[], const MPI_Offset stride[],
                    const MPI_Offset imap[], const float *op, int *request); 

int ncmpi_bput_varm_double(int ncid, int varid, const MPI_Offset start[],
                    const MPI_Offset count[], const MPI_Offset stride[],
                    const MPI_Offset imap[], const double *op, int *request); 

int ncmpi_bput_varm_longlong(int ncid, int varid, const MPI_Offset start[],
                    const MPI_Offset count[], const MPI_Offset stride[],
                    const MPI_Offset imap[], const long long *op, int *request); 

/* Begin Skip Prototypes for Fortran binding */

int ncmpi_iput_varm_uchar(int ncid, int varid, const MPI_Offset start[],
                    const MPI_Offset count[], const MPI_Offset stride[],
                    const MPI_Offset imap[], const unsigned char *op,
                    int *request);

int ncmpi_iput_varm_ushort(int ncid, int varid, const MPI_Offset start[],
                    const MPI_Offset count[], const MPI_Offset stride[],
                    const MPI_Offset imap[], const unsigned short *op,
                    int *request);

int ncmpi_iput_varm_uint(int ncid, int varid, const MPI_Offset start[],
                    const MPI_Offset count[], const MPI_Offset stride[],
                    const MPI_Offset imap[], const unsigned int *op,
                    int *request);

int ncmpi_iput_varm_long(int ncid, int varid, const MPI_Offset start[],
                    const MPI_Offset count[], const MPI_Offset stride[],
                    const MPI_Offset imap[], const long *op, int *request);

int ncmpi_iput_varm_ulonglong(int ncid, int varid, const MPI_Offset start[],
                    const MPI_Offset count[], const MPI_Offset stride[],
                    const MPI_Offset imap[], const unsigned long long *op,
                    int *request);

int ncmpi_iget_varm_uchar(int ncid, int varid, const MPI_Offset start[],
                    const MPI_Offset count[], const MPI_Offset stride[],
                    const MPI_Offset imap[], unsigned char *ip, int *request);

int ncmpi_iget_varm_ushort(int ncid, int varid, const MPI_Offset start[],
                    const MPI_Offset count[], const MPI_Offset stride[],
                    const MPI_Offset imap[], unsigned short *ip, int *request);

int ncmpi_iget_varm_uint(int ncid, int varid, const MPI_Offset start[],
                    const MPI_Offset count[], const MPI_Offset stride[],
                    const MPI_Offset imap[], unsigned int *ip, int *request);

int ncmpi_iget_varm_long(int ncid, int varid, const MPI_Offset start[],
                    const MPI_Offset count[], const MPI_Offset stride[],
                    const MPI_Offset imap[], long *ip, int *request);

int ncmpi_iget_varm_ulonglong(int ncid, int varid, const MPI_Offset start[],
                    const MPI_Offset count[], const MPI_Offset stride[],
                    const MPI_Offset imap[], unsigned long long *ip, int *request);

int ncmpi_bput_varm_uchar(int ncid, int varid, const MPI_Offset start[],
                    const MPI_Offset count[], const MPI_Offset stride[],
                    const MPI_Offset imap[], const unsigned char *op,
                    int *request);

int ncmpi_bput_varm_ushort(int ncid, int varid, const MPI_Offset start[],
                    const MPI_Offset count[], const MPI_Offset stride[],
                    const MPI_Offset imap[], const unsigned short *op,
                    int *request);

int ncmpi_bput_varm_uint(int ncid, int varid, const MPI_Offset start[],
                    const MPI_Offset count[], const MPI_Offset stride[],
                    const MPI_Offset imap[], const unsigned int *op,
                    int *request);

int ncmpi_bput_varm_long(int ncid, int varid, const MPI_Offset start[],
                    const MPI_Offset count[], const MPI_Offset stride[],
                    const MPI_Offset imap[], const long *op, int *request);

int ncmpi_bput_varm_ulonglong(int ncid, int varid, const MPI_Offset start[],
                    const MPI_Offset count[], const MPI_Offset stride[],
                    const MPI_Offset imap[], const unsigned long long *op,
                    int *request);

/* End Skip Prototypes for Fortran binding */

/* End {iput,iget,bput}_varm */

/* Begin {mput,mget}_var */

/* Begin Skip Prototypes for Fortran binding */

int ncmpi_mput_var(int ncid, int ntimes, int varids[],
                   void *bufs[], MPI_Offset bufcounts[],
                   MPI_Datatype buftypes[]);

int ncmpi_mput_var_all(int ncid, int ntimes, int varids[], 
                   void *bufs[], MPI_Offset bufcounts[],
                   MPI_Datatype buftypes[]);

int ncmpi_mput_var1(int ncid, int ntimes, int varids[],
                   MPI_Offset* const starts[],
                   void *bufs[], MPI_Offset bufcounts[],
                   MPI_Datatype buftypes[]);

int ncmpi_mput_var1_all(int ncid, int ntimes, int varids[], 
                   MPI_Offset* const starts[],
                   void *bufs[], MPI_Offset bufcounts[],
                   MPI_Datatype buftypes[]);

int ncmpi_mput_vara(int ncid, int ntimes, int varids[],
                   MPI_Offset* const starts[], MPI_Offset* const counts[],
                   void *bufs[], MPI_Offset bufcounts[],
                   MPI_Datatype buftypes[]);

int ncmpi_mput_vara_all(int ncid, int ntimes, int varids[], 
                   MPI_Offset* const starts[], MPI_Offset* const counts[], 
                   void *bufs[], MPI_Offset bufcounts[],
                   MPI_Datatype buftypes[]);

int ncmpi_mput_vars(int ncid, int ntimes, int varids[],
                   MPI_Offset* const starts[], MPI_Offset* const counts[],
                   MPI_Offset* const strides[],
                   void *bufs[], MPI_Offset bufcounts[],
                   MPI_Datatype buftypes[]);

int ncmpi_mput_vars_all(int ncid, int ntimes, int varids[], 
                   MPI_Offset* const starts[], MPI_Offset* const counts[], 
                   MPI_Offset* const strides[],
                   void *bufs[], MPI_Offset bufcounts[],
                   MPI_Datatype buftypes[]);

int ncmpi_mput_varm(int ncid, int ntimes, int varids[],
                   MPI_Offset* const starts[], MPI_Offset* const counts[],
                   MPI_Offset* const strides[], MPI_Offset* const imaps[],
                   void *bufs[], MPI_Offset bufcounts[],
                   MPI_Datatype buftypes[]);

int ncmpi_mput_varm_all(int ncid, int ntimes, int varids[], 
                   MPI_Offset* const starts[], MPI_Offset* const counts[], 
                   MPI_Offset* const strides[], MPI_Offset* const imaps[],
                   void *bufs[], MPI_Offset bufcounts[],
                   MPI_Datatype buftypes[]);

int ncmpi_mget_var(int ncid, int ntimes, int varids[], 
                   void *bufs[], MPI_Offset bufcounts[],
                   MPI_Datatype buftypes[]);

int ncmpi_mget_var_all(int ncid, int ntimes, int varids[], 
                   void *bufs[], MPI_Offset bufcounts[],
                   MPI_Datatype buftypes[]);

int ncmpi_mget_var1(int ncid, int ntimes, int varids[], 
                   MPI_Offset* const starts[],
                   void *bufs[], MPI_Offset bufcounts[],
                   MPI_Datatype buftypes[]);

int ncmpi_mget_var1_all(int ncid, int ntimes, int varids[], 
                   MPI_Offset* const starts[],
                   void *bufs[], MPI_Offset bufcounts[],
                   MPI_Datatype buftypes[]);

int ncmpi_mget_vara(int ncid, int ntimes, int varids[], 
                   MPI_Offset* const starts[], MPI_Offset* const counts[], 
                   void *bufs[], MPI_Offset bufcounts[],
                   MPI_Datatype buftypes[]);

int ncmpi_mget_vara_all(int ncid, int ntimes, int varids[], 
                   MPI_Offset* const starts[], MPI_Offset* const counts[], 
                   void *bufs[], MPI_Offset bufcounts[],
                   MPI_Datatype buftypes[]);

int ncmpi_mget_vars(int ncid, int ntimes, int varids[], 
                   MPI_Offset* const starts[], MPI_Offset* const counts[], 
                   MPI_Offset* const strides[],
                   void *bufs[], MPI_Offset bufcounts[],
                   MPI_Datatype buftypes[]);

int ncmpi_mget_vars_all(int ncid, int ntimes, int varids[], 
                   MPI_Offset* const starts[], MPI_Offset* const counts[], 
                   MPI_Offset* const strides[],
                   void *bufs[], MPI_Offset bufcounts[],
                   MPI_Datatype buftypes[]);

int ncmpi_mget_varm(int ncid, int ntimes, int varids[], 
                   MPI_Offset* const starts[], MPI_Offset* const counts[], 
                   MPI_Offset* const strides[], MPI_Offset* const imaps[],
                   void *bufs[], MPI_Offset bufcounts[],
                   MPI_Datatype buftypes[]);

int ncmpi_mget_varm_all(int ncid, int ntimes, int varids[], 
                   MPI_Offset* const starts[], MPI_Offset* const counts[], 
                   MPI_Offset* const strides[], MPI_Offset* const imaps[],
                   void *bufs[], MPI_Offset bufcounts[],
                   MPI_Datatype buftypes[]);

/* End Skip Prototypes for Fortran binding */

/* End {mput,mget}_var */

int ncmpi_buffer_attach(int ncid, MPI_Offset bufsize);
int ncmpi_buffer_detach(int ncid);
int ncmpi_inq_buffer_usage(int ncid, MPI_Offset *usage);


/* End non-blocking data access functions */

/* End: more prototypes to be included for fortran binding conversion */
/* ################################################################## */

/* End Prototypes */


/* These macros are defined in serial netcdf (3.5.0) for backwards
 * compatibility with older netcdf code.   We aren't concerned with backwards
 * compatibility, so if your code doesn't compile with parallel-netcdf, maybe
 * this is why: 
 *
 *
 *  OLD NAME                 NEW NAME
 *  ----------------------------------
 *  FILL_BYTE       NC_FILL_BYTE
 *  FILL_CHAR       NC_FILL_CHAR
 *  FILL_SHORT      NC_FILL_SHORT
 *  FILL_LONG       NC_FILL_INT
 *  FILL_FLOAT      NC_FILL_FLOAT
 *  FILL_DOUBLE     NC_FILL_DOUBLE
 *
 *  MAX_NC_DIMS     NC_MAX_DIMS
 *  MAX_NC_ATTRS    NC_MAX_ATTRS
 *  MAX_NC_VARS     NC_MAX_VARS
 *  MAX_NC_NAME     NC_MAX_NAME
 *  MAX_VAR_DIMS    NC_MAX_VAR_DIMS
 */

#if defined(__cplusplus)
}
#endif
#endif

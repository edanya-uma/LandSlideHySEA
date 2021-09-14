#ifndef _CONSTANTES_H_
#define _CONSTANTES_H_

/***************************/
/* Constantes de CPU y GPU */
/***************************/

#include <cuda.h>
#include <cuda_runtime.h>
#include <float.h>

#define NUM_VARIABLES  6  // h1, q1x, q1y, h2, q2x, q2y
#define EPSILON   FLT_EPSILON
#define SGN(x)  ((fabsf(x) < EPSILON) ? 0 : ((x) > 0) ? 1 : -1 )

// COULOMB / POULIQUEN
#define COULOMB

// Tipo escalar usado en CPU
typedef double Scalar;

// Tipo de los datos que se utilizan en un cluster (se utiliza en MultiGPU)
typedef struct TDatoCluster {
	// N�mero de vol�menes en x e y que tiene el cluster
	int num_volx, num_voly;
	// Datos de los vol�menes (en datosVolumenes_1 se almacena h1, q1x, q1y y H,
	// mientras que en datosVolumenes_2 se almacena h2, q2x, q2y y el �rea del volumen)
	float4 *datosVolumenes_1, *datosVolumenes_2;
	// En la componente x se almacenar� la eta1 m�xima. En la componente y
	// se almacenar� el tiempo en segundos en el que se ha alcanzado
	float2 *eta1_maxima;

	// Datos de los vol�menes de comunicaci�n del cluster
	float4 *puntero_datosVolumenesComClusterSup_1;
	float4 *puntero_datosVolumenesComClusterSup_2;
	float4 *puntero_datosVolumenesComClusterInf_1;
	float4 *puntero_datosVolumenesComClusterInf_2;

	float4 *puntero_datosVolumenesComOtroClusterSup_1;
	float4 *puntero_datosVolumenesComOtroClusterSup_2;
	float4 *puntero_datosVolumenesComOtroClusterInf_1;
	float4 *puntero_datosVolumenesComOtroClusterInf_2;
} TDatoCluster;

typedef struct TSW_Cuda {
	// Array d_datosVolumenes (donde se almacenar�n W y H).
	cudaArray *d_datosVolumenes_1, *d_datosVolumenes_2;
	float2 *d_eta1_maxima;
	// Punteros que apuntan al principio de los vol�menes de comunicaci�n del cluster
	// y de los clusters adyacentes.
	float4 *d_datosVolumenesComClusterSup_1, *d_datosVolumenesComClusterSup_2;
	float4 *d_datosVolumenesComClusterInf_1, *d_datosVolumenesComClusterInf_2;
	float4 *d_datosVolumenesComOtroClusterSup_1, *d_datosVolumenesComOtroClusterSup_2;
	float4 *d_datosVolumenesComOtroClusterInf_1, *d_datosVolumenesComOtroClusterInf_2;

	// Acumuladores donde, para cada volumen, sus aristas almacenar�n sus contribuciones.
	// Para el volumen i-�simo, d_acumulador1[i] almacena la contribuci�n a h1, q1x, q1y y
	// delta_T, mientras que d_acumulador2[i] almacena la contribuci�n a h2, q2x y q2y.
	float4 *d_acumulador1, *d_acumulador2;
	// Array donde, para cada volumen, se almacenar� su delta T local. Los vol�menes
	// de comunicaci�n est�n al final del array
	float *d_deltaTVolumenes;

	// Tama�o del grid y de bloque en el procesamiento de aristas que no son de comunicaci�n
	// (al procesar las aristas Hor1, se procesan las mismas aristas que en el caso sin solapamiento
	// menos la primera y �ltima fila. El resto de procesamientos de aristas es igual)
	dim3 blockGridVer1, blockGridVer2;
	dim3 blockGridHor1, blockGridHor2, threadBlockAri;
	// Tama�o del grid y de bloque en el procesamiento de aristas de comunicaci�n
	dim3 blockGridHorCom, threadBlockAriCom;
	// Tama�o del grid y bloques en el procesamiento de los vol�menes
	dim3 blockGridDeltaT, threadBlockDeltaT;
	dim3 blockGridEst, threadBlockEst;
} TSW_Cuda;

#ifdef CONSTANTES_GPU

/*********************/
/* Constantes de GPU */
/*********************/

// Tama�o de un bloque en el procesamiento de las aristas
#define NUM_HEBRAS_ANCHO_ARI 8
#define NUM_HEBRAS_ALTO_ARI  8
// Anacho de un bloque en el procesamiento de las aristas de comunicaci�n (alto==2)
#define NUM_HEBRAS_ANCHO_ARI_COM 32
// Tama�o de un bloque en el c�lculo del delta_T
#define NUM_HEBRAS_VOL 256
// Tama�o de un bloque en el c�lculo del nuevo estado
#define NUM_HEBRAS_ANCHO_EST 8
#define NUM_HEBRAS_ALTO_EST  8

// Texturas que contienen los datos de los vol�menes para las capas
// 1 y 2. H est� en ambas texturas
texture<float4, 2, cudaReadModeElementType> texDatosVolumenes_1;
texture<float4, 2, cudaReadModeElementType> texDatosVolumenes_2;

// Redondea a / b al mayor entero m�s cercano
#define iDivUp(a,b)  (((a)%(b) != 0) ? ((a)/(b) + 1) : ((a)/(b)))

// Redondea a / b al menor entero m�s cercano
#define iDivDown(a,b)  ((a)/(b))

// Alinea a al mayor entero m�ltiplo de b m�s cercano
#define iAlignUp(a,b)  (((a)%(b) != 0) ? ((a)-(a) % (b)+(b)) : (a))

// Alinea a al menor entero m�ltiplo de b m�s cercano
#define iAlignDown(a,b)  ((a)-(a) % (b))

#else	// #ifdef CONSTANTES_GPU

/*********************/
/* Constantes de CPU */
/*********************/

#include <iostream>
#include <iomanip>
#define _USE_MATH_DEFINES
#include <math.h>

using namespace std;

#endif	// #ifdef CONSTANTES_GPU

#endif

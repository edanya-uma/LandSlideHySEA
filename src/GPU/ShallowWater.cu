#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <fstream>
#include <mpi.h>
#include "Arista_kernel.cu"
#include "Reduccion_kernel.cu"
#include "Volumen_kernel.cu"
#include "netcdf.cu"

using namespace std;

// Devuelve 0 si todo ha ido bien, 1 si no hay memoria GPU suficiente.
int inicializarDatosCuda(TDatoCluster *datos_cluster, TSW_Cuda *datos_SW_Cuda, int id_hebra)
{
	int num_aristas_ver1, num_aristas_ver2;
	int num_aristas_hor1, num_aristas_hor2;
	int num_volx = datos_cluster->num_volx;
	int num_voly = datos_cluster->num_voly;
	int num_volumenes = num_volx*num_voly;
	int tam_datosVolumenes = num_volumenes * sizeof(float4);
	int tam_datosVolComFloat4 = num_volx * sizeof(float4);
	int tam_datosEta1 = num_volumenes * sizeof(float2);
	int tam_datosDeltaT = num_volumenes * sizeof(float);
	cudaChannelFormatDesc float4Tex_1 = cudaCreateChannelDesc<float4>();
	cudaChannelFormatDesc float4Tex_2 = cudaCreateChannelDesc<float4>();
	cudaError_t err_cuda;

	// Asignamos la GPU
//	cudaSetDevice(0);
	cudaSetDevice(id_hebra%2);

	// N�mero de aristas verticales y horizontales
	num_aristas_ver1 = (num_volx/2 + 1)*num_voly;
	num_aristas_ver2 = (num_volx % 2 == 0) ? num_volx*num_voly/2 : num_aristas_ver1;
	num_aristas_hor1 = (num_voly/2 + 1)*num_volx;
	num_aristas_hor2 = (num_voly % 2 == 0) ? num_volx*num_voly/2 : num_aristas_hor1;

	// Reservamos memoria en GPU para los acumuladores, los datos de los vol�menes
	// y el array de los delta T de los vol�menes.
	// Datos de los vol�menes
	cudaMallocArray(&(datos_SW_Cuda->d_datosVolumenes_1), &float4Tex_1, num_volx, num_voly+2);
	cudaMallocArray(&(datos_SW_Cuda->d_datosVolumenes_2), &float4Tex_2, num_volx, num_voly+2);
	cudaMalloc( (void **)&datos_SW_Cuda->d_eta1_maxima, tam_datosEta1);
	// Delta T de los vol�menes
	cudaMalloc( (void **)&datos_SW_Cuda->d_deltaTVolumenes, tam_datosDeltaT);
	// Acumuladores
	cudaMalloc( (void **)&datos_SW_Cuda->d_acumulador1, tam_datosVolumenes);
	err_cuda = cudaMalloc( (void **)&datos_SW_Cuda->d_acumulador2, tam_datosVolumenes);
	if (err_cuda == cudaErrorMemoryAllocation) {
		cudaFree(datos_SW_Cuda->d_acumulador1);
		cudaFree(datos_SW_Cuda->d_deltaTVolumenes);
		cudaFree(datos_SW_Cuda->d_datosVolumenes_1);
		cudaFree(datos_SW_Cuda->d_datosVolumenes_2);
		cudaFree(datos_SW_Cuda->d_eta1_maxima);
		return 1;
	}

	// Tama�os del grid y de bloque en el procesamiento de aristas que no son de comunicaci�n
	datos_SW_Cuda->blockGridVer1.x = iDivUp(num_aristas_ver1/num_voly, NUM_HEBRAS_ANCHO_ARI);
	datos_SW_Cuda->blockGridVer1.y = iDivUp(num_voly, NUM_HEBRAS_ALTO_ARI);

	datos_SW_Cuda->blockGridVer2.x = iDivUp(num_aristas_ver2/num_voly, NUM_HEBRAS_ANCHO_ARI);
	datos_SW_Cuda->blockGridVer2.y = iDivUp(num_voly, NUM_HEBRAS_ALTO_ARI);

	datos_SW_Cuda->blockGridHor1.x = iDivUp(num_volx, NUM_HEBRAS_ANCHO_ARI);
	datos_SW_Cuda->blockGridHor1.y = iDivUp(num_aristas_hor1/num_volx, NUM_HEBRAS_ALTO_ARI);

	datos_SW_Cuda->blockGridHor2.x = iDivUp(num_volx, NUM_HEBRAS_ANCHO_ARI);
	datos_SW_Cuda->blockGridHor2.y = iDivUp(num_aristas_hor2/num_volx, NUM_HEBRAS_ALTO_ARI);

	datos_SW_Cuda->threadBlockAri.x = NUM_HEBRAS_ANCHO_ARI;
	datos_SW_Cuda->threadBlockAri.y = NUM_HEBRAS_ALTO_ARI;

	// Tama�os del grid y de bloque en el procesamiento de aristas de comunicaci�n
	datos_SW_Cuda->blockGridHorCom.x = iDivUp(num_volx, NUM_HEBRAS_ANCHO_ARI_COM);
	datos_SW_Cuda->blockGridHorCom.y = 1;

	datos_SW_Cuda->threadBlockAriCom.x = NUM_HEBRAS_ANCHO_ARI_COM;
	datos_SW_Cuda->threadBlockAriCom.y = 2;

	// Tama�o del grid y bloques en el c�lculo del delta_T
	datos_SW_Cuda->blockGridDeltaT.x = iDivUp(num_volumenes, NUM_HEBRAS_VOL);
	datos_SW_Cuda->blockGridDeltaT.y = 1;

	datos_SW_Cuda->threadBlockDeltaT.x = NUM_HEBRAS_VOL;
	datos_SW_Cuda->threadBlockDeltaT.y = 1;

	// Tama�o del grid y bloques en el c�lculo del nuevo estado
	datos_SW_Cuda->blockGridEst.x = iDivUp(num_volx, NUM_HEBRAS_ANCHO_EST);
	datos_SW_Cuda->blockGridEst.y = iDivUp(num_voly, NUM_HEBRAS_ALTO_EST);

	datos_SW_Cuda->threadBlockEst.x = NUM_HEBRAS_ANCHO_EST;
	datos_SW_Cuda->threadBlockEst.y = NUM_HEBRAS_ALTO_EST;

	// Copiamos los datos de los vol�menes de CPU a GPU
	// Hay fila de vol�menes de comunicaci�n de otro cluster en la parte superior
	cudaMemcpyToArray(datos_SW_Cuda->d_datosVolumenes_1, 0, 0, datos_cluster->datosVolumenes_1, tam_datosVolumenes + 2*tam_datosVolComFloat4,
		cudaMemcpyHostToDevice);
	cudaMemcpyToArray(datos_SW_Cuda->d_datosVolumenes_2, 0, 0, datos_cluster->datosVolumenes_2, tam_datosVolumenes + 2*tam_datosVolComFloat4,
		cudaMemcpyHostToDevice);
	cudaBindTextureToArray(texDatosVolumenes_1, datos_SW_Cuda->d_datosVolumenes_1);
	cudaBindTextureToArray(texDatosVolumenes_2, datos_SW_Cuda->d_datosVolumenes_2);
	cudaMemcpy(datos_SW_Cuda->d_eta1_maxima, datos_cluster->eta1_maxima, tam_datosEta1, cudaMemcpyHostToDevice);
	// Inicializamos los acumuladores
	cudaMemset(datos_SW_Cuda->d_acumulador1, 0, tam_datosVolumenes);
	cudaMemset(datos_SW_Cuda->d_acumulador2, 0, tam_datosVolumenes);

	return 0;
}

void liberarSWCuda(TSW_Cuda *datos_SW_Cuda)
{
	cudaUnbindTexture(texDatosVolumenes_1);
	cudaUnbindTexture(texDatosVolumenes_2);
	cudaFree(datos_SW_Cuda->d_acumulador1);
	cudaFree(datos_SW_Cuda->d_acumulador2);
	cudaFree(datos_SW_Cuda->d_eta1_maxima);
	cudaFree(datos_SW_Cuda->d_deltaTVolumenes);
	cudaFreeArray(datos_SW_Cuda->d_datosVolumenes_1);
	cudaFreeArray(datos_SW_Cuda->d_datosVolumenes_2);
}

// Devuelve 0 si todo ha ido bien, 1 si no hay memoria GPU suficiente, y 2 si no hay memoria CPU suficiente
extern "C" int shallowWater(TDatoCluster *datos_cluster, float xmin, float ymin, float Hmin, char *nombre_bati,
		char * prefijo, int num_voly_otros, int num_voly_total, float borde_sup, float borde_inf, float borde_izq,
		float borde_der, float ancho_vol, float alto_vol, float area, float tiempo_tot, float tiempo_guardar,
		float CFL, float r, float angulo1, float angulo2, float angulo3, float angulo4, float peso, float beta,
		float mfc, float mf0, float mfs, float vmax1, float vmax2, float gravedad, float epsilon_h, float L,
		float H, float Q, float T, int num_procs, int id_hebra, double *tiempo, int leer_fichero_puntos, 
		int *indiceVolumenesGuardado, int *posicionesVolumenesGuardado, int num_puntos_guardar)
{
	double tiempo_ini, tiempo_fin;
	int err, err_total;
	MPI_Request request_1[2], request_2[2];
	MPI_Request request2;
	MPI_Status status[2];
	float *vec;
	float4 datos1, datos2;
	// Tipos para transmitir datos en MPI
	MPI_Datatype tipo_float4;
	MPI_Datatype tipo[4];
	int blocklen[4] = {1, 1, 1, 1};
	MPI_Aint disp[4];
	// Datos utilizados en Cuda por el cluster (punteros a memoria global
	// y tama�o de bloques)
	TSW_Cuda datos_SW_Cuda;
	int i, j, pos;
	// N�mero del estado que se va guardando
	int num = 0;

	int num_volx = datos_cluster->num_volx;
	int num_voly = datos_cluster->num_voly;
	int hebra_ant = id_hebra-1;
	int hebra_sig = id_hebra+1;
	int ultima_hebra = (id_hebra == num_procs-1) ? 1 : 0;
	int num_volumenes = num_volx*num_voly;
	// nvolx y nvoly que se guardan en NetCDF
	int nx_nc, ny_nc;
	// iniy: coordenada y local a datosVolumenes a partir de la que se guardar�n puntos
	int iniy, iniy_nc;
	int npics = 1;
	char nombre_fich[512];
	int tam_datosVolumenes = num_volumenes * sizeof(float4);
	int tam_datosVolComFloat4 = num_volx * sizeof(float4);
	int tam_datosEta1 = num_volumenes * sizeof(float2);
	int tam_datosVolGuardadoFloat4 = num_puntos_guardar*sizeof(float4);
	float tiempo_act, delta_T, dT_min;
	float sig_tiempo_guardar = 0.0;
	int iter;

	int *d_posicionesVolumenesGuardado;
        float4 *d_datosVolumenesGuardado_1;

	FILE *fp;

	// Tipo tipo_float4
	tipo[0] = MPI_FLOAT;
	tipo[1] = MPI_FLOAT;
	tipo[2] = MPI_FLOAT;
	tipo[3] = MPI_FLOAT;
	disp[0] = 0;
	disp[1] = sizeof(float);
	disp[2] = 2*sizeof(float);
	disp[3] = 3*sizeof(float);

	MPI_Type_struct(4, blocklen, disp, tipo, &tipo_float4);
	MPI_Type_commit(&tipo_float4);

	// Inicializamos los datos en cada GPU
	err = inicializarDatosCuda(datos_cluster, &datos_SW_Cuda, id_hebra);

	// Comprobamos si se ha producido un error en alg�n proceso
	MPI_Allreduce (&err, &err_total, 1, MPI_INT, MPI_MAX, MPI_COMM_WORLD);

	// Fijamos los tama�os relativos de la cach� L1 y la memoria compartida
	cudaFuncSetCacheConfig(procesarAristasGPU, cudaFuncCachePreferL1);

	if (leer_fichero_puntos == 1) {
                cudaMalloc((void **)&d_posicionesVolumenesGuardado, num_volumenes*sizeof(int));
                cudaMalloc((void **)&d_datosVolumenesGuardado_1, tam_datosVolGuardadoFloat4);
	}
	
	if (leer_fichero_puntos == 1)
                cudaMemcpy(d_posicionesVolumenesGuardado, posicionesVolumenesGuardado, num_volumenes*sizeof(int), cudaMemcpyHostToDevice);

	if (err_total != 1) {
		MPI_Barrier(MPI_COMM_WORLD);

		// Inicio NetCDF
		if(leer_fichero_puntos==0) {
			vec = (float *) malloc(num_volumenes*sizeof(float));
			if (vec == NULL) {
				liberarSWCuda(&datos_SW_Cuda);
				return 2;
			}
			for (i=0; i<num_volumenes; i++) {
				datos1 = datos_cluster->datosVolumenes_1[num_volx+i];
				vec[i] = (datos1.w + Hmin)*H;
			}
			double fac = (Q/H)*sqrt(L)/pow((double) H, (double) 7.0/6.0);
			initNC(id_hebra, nombre_bati, prefijo, num_volx, num_voly, num_voly_otros, num_voly_total, &nx_nc, &ny_nc,
				npics, xmin*L, ymin*L, ancho_vol*L, alto_vol*L, tiempo_tot*T, CFL, r, angulo1*180.0/M_PI, angulo2*180.0/M_PI,
				angulo3*180.0/M_PI, angulo4*180.0/M_PI, mfc/L, mf0/fac, mfs/fac, vmax1*Q/H, vmax2*Q/H, vec);
			// Reasignamos ny_nc para que sea local al cluster
			for (iniy=id_hebra*num_voly_otros; iniy%npics != 0; iniy++);
			iniy = iniy - id_hebra*num_voly_otros;
			iniy_nc = (id_hebra*num_voly_otros-1)/npics + 1;
			ny_nc = (num_voly-1-iniy)/npics + 1;
		}else{
			sprintf(nombre_fich, "%s_eta_puntos.txt", prefijo);
                	fp = fopen(nombre_fich, "wt");
		}
		// Fin NetCDF

		// C�LCULO DEL DELTA_T INICIAL
		// Procesamos las aristas horizontales
		procesarAristasDeltaTInicialGPU<<<datos_SW_Cuda.blockGridHor1, datos_SW_Cuda.threadBlockAri>>>(num_volx, num_voly, num_volumenes,
			borde_sup, borde_inf, ancho_vol, r, datos_SW_Cuda.d_acumulador1, gravedad, epsilon_h, 3, id_hebra, ultima_hebra);
		procesarAristasDeltaTInicialGPU<<<datos_SW_Cuda.blockGridHor2, datos_SW_Cuda.threadBlockAri>>>(num_volx, num_voly, num_volumenes,
			borde_sup, borde_inf, ancho_vol, r, datos_SW_Cuda.d_acumulador1, gravedad, epsilon_h, 4, id_hebra, ultima_hebra);

		// Procesamos las aristas verticales
		procesarAristasDeltaTInicialGPU<<<datos_SW_Cuda.blockGridVer1, datos_SW_Cuda.threadBlockAri>>>(num_volx, num_voly, num_volumenes,
			borde_izq, borde_der, alto_vol, r, datos_SW_Cuda.d_acumulador1, gravedad, epsilon_h, 1, id_hebra, ultima_hebra);
		procesarAristasDeltaTInicialGPU<<<datos_SW_Cuda.blockGridVer2, datos_SW_Cuda.threadBlockAri>>>(num_volx, num_voly, num_volumenes,
			borde_izq, borde_der, alto_vol, r, datos_SW_Cuda.d_acumulador1, gravedad, epsilon_h, 2, id_hebra, ultima_hebra);

		// Obtenemos el delta T local de cada volumen
		obtenerDeltaTVolumenesGPU<<<datos_SW_Cuda.blockGridDeltaT, datos_SW_Cuda.threadBlockDeltaT>>>(datos_SW_Cuda.d_acumulador1,
			datos_SW_Cuda.d_deltaTVolumenes, num_volumenes, area, CFL);

		// Obtenemos el m�nimo delta T del cluster aplicando un algoritmo de reducci�n
		dT_min = obtenerMinimoReduccion<float>(datos_SW_Cuda.d_deltaTVolumenes, num_volumenes);

		// Obtenemos el m�nimo delta T de todos los clusters por reducci�n
		MPI_Allreduce (&dT_min, &delta_T, 1, MPI_FLOAT, MPI_MIN, MPI_COMM_WORLD);
//delta_T=5e-4/T;
		if (id_hebra == 0)
			fprintf(stdout, "deltaT inicial = %e seg\n", delta_T*T);

		// Reinicializamos el acumulador1
		cudaMemset(datos_SW_Cuda.d_acumulador1, 0, tam_datosVolumenes);

		MPI_Barrier(MPI_COMM_WORLD);
		iter = 1;
		tiempo_ini = MPI_Wtime();
		tiempo_act = 0.0;
		while (tiempo_act < tiempo_tot) {
			// Guardamos el estado actual, si procede
			// Inicio NetCDF
			if ((tiempo_guardar >= 0.0) && (tiempo_act >= sig_tiempo_guardar)) {
			if(leer_fichero_puntos == 0) {
				cudaMemcpyFromArray(datos_cluster->datosVolumenes_1, datos_SW_Cuda.d_datosVolumenes_1, 0, 1, tam_datosVolumenes, cudaMemcpyDeviceToHost);
				cudaMemcpyFromArray(datos_cluster->datosVolumenes_2, datos_SW_Cuda.d_datosVolumenes_2, 0, 1, tam_datosVolumenes, cudaMemcpyDeviceToHost);
				for (j=0; j<ny_nc; j++) {
					pos = (iniy + j*npics)*num_volx;
					for (i=0; i<nx_nc; i++) {
						datos1 = datos_cluster->datosVolumenes_1[pos + i*npics];
						datos2 = datos_cluster->datosVolumenes_2[pos + i*npics];
						vec[j*nx_nc + i] = (datos1.x + datos2.x - datos1.w - Hmin)*H;
					}
				}
				writeEta1NC(nx_nc, ny_nc, iniy_nc, num, tiempo_act*T, vec);
				for (j=0; j<ny_nc; j++) {
					pos = (iniy + j*npics)*num_volx;
					for (i=0; i<nx_nc; i++)
						vec[j*nx_nc + i] = (datos_cluster->datosVolumenes_1[pos + i*npics].y)*Q;
				}
				writeQ1xNC(nx_nc, ny_nc, iniy_nc, num, tiempo_act*T, vec);
				for (j=0; j<ny_nc; j++) {
					pos = (iniy + j*npics)*num_volx;
					for (i=0; i<nx_nc; i++)
						vec[j*nx_nc + i] = (datos_cluster->datosVolumenes_1[pos + i*npics].z)*Q;
				}
				writeQ1yNC(nx_nc, ny_nc, iniy_nc, num, tiempo_act*T, vec);
				for (j=0; j<ny_nc; j++) {
					pos = (iniy + j*npics)*num_volx;
					for (i=0; i<nx_nc; i++) {
						datos2 = datos_cluster->datosVolumenes_2[pos + i*npics];
						vec[j*nx_nc + i] = (datos2.x)*H;//(datos2.x - datos2.w - Hmin)*H;
					}
				}
				writeEta2NC(nx_nc, ny_nc, iniy_nc, num, tiempo_act*T, vec);
				for (j=0; j<ny_nc; j++) {
					pos = (iniy + j*npics)*num_volx;
					for (i=0; i<nx_nc; i++)
						vec[j*nx_nc + i] = (datos_cluster->datosVolumenes_2[pos + i*npics].y)*Q;
				}
				writeQ2xNC(nx_nc, ny_nc, iniy_nc, num, tiempo_act*T, vec);
				for (j=0; j<ny_nc; j++) {
					pos = (iniy + j*npics)*num_volx;
					for (i=0; i<nx_nc; i++)
						vec[j*nx_nc + i] = (datos_cluster->datosVolumenes_2[pos + i*npics].z)*Q;
				}
				writeQ2yNC(nx_nc, ny_nc, iniy_nc, num, tiempo_act*T, vec);
				num++;
			} else {

				cudaMemcpyFromArray(datos_cluster->datosVolumenes_1, datos_SW_Cuda.d_datosVolumenes_1, 0, 1, tam_datosVolumenes, cudaMemcpyDeviceToHost);
                                fprintf(fp, "%e", tiempo_act*T);
                                for (i=0; i<num_puntos_guardar; i++) {
                                        if (indiceVolumenesGuardado[i] != -1) {
                                                j = indiceVolumenesGuardado[i];
                                                fprintf(fp, " %.8e", (datos_cluster->datosVolumenes_1[j].x - datos_cluster->datosVolumenes_1[j].w - Hmin)*H);
                                        }
                                        else
                                                fprintf(fp, " -999");   
                                }
                                fprintf(fp, "\n");

			}
			sig_tiempo_guardar += tiempo_guardar;
			}
			// Fin NetCDF

			// Actualizamos los valores m�ximos de eta1 y sus tiempos asociados
			actualizarEta1MaximaGPU<<<datos_SW_Cuda.blockGridEst, datos_SW_Cuda.threadBlockEst>>>(datos_SW_Cuda.d_eta1_maxima,
				num_volx, num_voly, tiempo_act);

			// SOLAPAMIENTO MPI-cudaMemcpy-computaci�n
			// Recibimos de los clusters adyacentes sus vol�menes de comunicaci�n adyacentes a nuestro cluster.
			if (id_hebra != 0) {
				// Es una hebra distinta de la primera.
				// Recibimos los vol�menes de comunicaci�n inferiores del cluster superior
				MPI_Irecv(datos_cluster->puntero_datosVolumenesComOtroClusterInf_1, num_volx, tipo_float4, hebra_ant, 22,
					MPI_COMM_WORLD, request_1);
				MPI_Irecv(datos_cluster->puntero_datosVolumenesComOtroClusterInf_2, num_volx, tipo_float4, hebra_ant, 23,
					MPI_COMM_WORLD, request_1+1);
			}
			if (! ultima_hebra) {
				// Es una hebra distinta de la �ltima.
				// Recibimos los vol�menes de comunicaci�n superiores del cluster inferior
				MPI_Irecv(datos_cluster->puntero_datosVolumenesComOtroClusterSup_1, num_volx, tipo_float4, hebra_sig, 22,
					MPI_COMM_WORLD, request_2);
				MPI_Irecv(datos_cluster->puntero_datosVolumenesComOtroClusterSup_2, num_volx, tipo_float4, hebra_sig, 23,
					MPI_COMM_WORLD, request_2+1);
			}
			// Copiamos los vol�menes de comunicaci�n del cluster a memoria CPU
			// Vol�menes de comunicaci�n superiores
			cudaMemcpyFromArray(datos_cluster->puntero_datosVolumenesComClusterSup_1, datos_SW_Cuda.d_datosVolumenes_1, 0, 1,
				tam_datosVolComFloat4, cudaMemcpyDeviceToHost);
			cudaMemcpyFromArray(datos_cluster->puntero_datosVolumenesComClusterSup_2, datos_SW_Cuda.d_datosVolumenes_2, 0, 1,
				tam_datosVolComFloat4, cudaMemcpyDeviceToHost);
			// Vol�menes de comunicaci�n inferiores
			cudaMemcpyFromArray(datos_cluster->puntero_datosVolumenesComClusterInf_1, datos_SW_Cuda.d_datosVolumenes_1, 0, num_voly,
				tam_datosVolComFloat4, cudaMemcpyDeviceToHost);
			cudaMemcpyFromArray(datos_cluster->puntero_datosVolumenesComClusterInf_2, datos_SW_Cuda.d_datosVolumenes_2, 0, num_voly,
				tam_datosVolComFloat4, cudaMemcpyDeviceToHost);

			// Enviamos a los procesos asociados a los clusters adyacentes a nuestro cluster
			// los vol�menes de comunicaci�n correspondientes de nuestro cluster.
			if (! ultima_hebra) {
				// Es una hebra distinta de la �ltima.
				// Enviamos los vol�menes de comunicaci�n inferiores al cluster inferior
				MPI_Isend(datos_cluster->puntero_datosVolumenesComClusterInf_1, num_volx, tipo_float4, hebra_sig, 22,
					MPI_COMM_WORLD, &request2);
				MPI_Isend(datos_cluster->puntero_datosVolumenesComClusterInf_2, num_volx, tipo_float4, hebra_sig, 23,
					MPI_COMM_WORLD, &request2);
			}
			if (id_hebra != 0) {
				// Es una hebra distinta de la primera.
				// Enviamos los vol�menes de comunicaci�n superiores al cluster superior
				MPI_Isend(datos_cluster->puntero_datosVolumenesComClusterSup_1, num_volx, tipo_float4, hebra_ant, 22,
					MPI_COMM_WORLD, &request2);
				MPI_Isend(datos_cluster->puntero_datosVolumenesComClusterSup_2, num_volx, tipo_float4, hebra_ant, 23,
					MPI_COMM_WORLD, &request2);
			}

			// Procesamos las aristas de Hor1 que no son de comunicaci�n
			procesarAristasNoComGPU<<<datos_SW_Cuda.blockGridHor1, datos_SW_Cuda.threadBlockAri>>>(num_volx, num_voly,
				num_volumenes, borde_sup, borde_inf, ancho_vol, area, r, delta_T, angulo1, angulo2, angulo3, angulo4, peso, beta,
				datos_SW_Cuda.d_acumulador1, datos_SW_Cuda.d_acumulador2, gravedad, epsilon_h, L, H, 3, id_hebra, ultima_hebra);

			// Esperamos a que hayamos recibido los vol�menes de comunicaci�n de todos los clusters adyacentes
			if (id_hebra != 0)
				MPI_Waitall(2, request_1, status);
			if (! ultima_hebra)
				MPI_Waitall(2, request_2, status);

			// Copiamos los vol�menes de comunicaci�n recibidos a memoria GPU
			// Vol�menes de comunicaci�n inferiores del cluster superior
			cudaMemcpyToArray(datos_SW_Cuda.d_datosVolumenes_1, 0, 0, datos_cluster->puntero_datosVolumenesComOtroClusterInf_1,
				tam_datosVolComFloat4, cudaMemcpyHostToDevice);
			cudaMemcpyToArray(datos_SW_Cuda.d_datosVolumenes_2, 0, 0, datos_cluster->puntero_datosVolumenesComOtroClusterInf_2,
				tam_datosVolComFloat4, cudaMemcpyHostToDevice);
			// Vol�menes de comunicaci�n superiores del cluster inferior
			cudaMemcpyToArray(datos_SW_Cuda.d_datosVolumenes_1, 0, num_voly+1, datos_cluster->puntero_datosVolumenesComOtroClusterSup_1,
				tam_datosVolComFloat4, cudaMemcpyHostToDevice);
			cudaMemcpyToArray(datos_SW_Cuda.d_datosVolumenes_2, 0, num_voly+1, datos_cluster->puntero_datosVolumenesComOtroClusterSup_2,
				tam_datosVolComFloat4, cudaMemcpyHostToDevice);

			// Procesamos las aristas horizontales (en el caso de Hor1 s�lo las de comunicaci�n)
			procesarAristasComGPU<<<datos_SW_Cuda.blockGridHorCom, datos_SW_Cuda.threadBlockAriCom>>>(num_volx, num_voly,
				num_volumenes, borde_sup, borde_inf, ancho_vol, area, r, delta_T, angulo1, angulo2, angulo3, angulo4, peso, beta,
				datos_SW_Cuda.d_acumulador1, datos_SW_Cuda.d_acumulador2, gravedad, epsilon_h, L, H, 3, id_hebra, ultima_hebra);
			procesarAristasGPU<<<datos_SW_Cuda.blockGridHor2, datos_SW_Cuda.threadBlockAri>>>(num_volx, num_voly, num_volumenes,
				borde_sup, borde_inf, ancho_vol, area, r, delta_T, angulo1, angulo2, angulo3, angulo4, peso, beta,
				datos_SW_Cuda.d_acumulador1, datos_SW_Cuda.d_acumulador2, gravedad, epsilon_h, L, H, 4, id_hebra, ultima_hebra);

			// Procesamos las aristas verticales
			procesarAristasGPU<<<datos_SW_Cuda.blockGridVer1, datos_SW_Cuda.threadBlockAri>>>(num_volx, num_voly, num_volumenes,
				borde_izq, borde_der, alto_vol, area, r, delta_T, angulo1, angulo2, angulo3, angulo4, peso, beta,
				datos_SW_Cuda.d_acumulador1, datos_SW_Cuda.d_acumulador2, gravedad, epsilon_h, L, H, 1, id_hebra, ultima_hebra);
			procesarAristasGPU<<<datos_SW_Cuda.blockGridVer2, datos_SW_Cuda.threadBlockAri>>>(num_volx, num_voly, num_volumenes,
				borde_izq, borde_der, alto_vol, area, r, delta_T, angulo1, angulo2, angulo3, angulo4, peso, beta,
				datos_SW_Cuda.d_acumulador1, datos_SW_Cuda.d_acumulador2, gravedad, epsilon_h, L, H, 2, id_hebra, ultima_hebra);

			// Actualizamos en d_acumulador_1 y d_acumulador_2 el estado de cada volumen
			// Obtenemos tambi�n el delta T local de cada volumen
			obtenerEstadoYDeltaTVolumenesGPU<<<datos_SW_Cuda.blockGridEst, datos_SW_Cuda.threadBlockEst>>>(datos_SW_Cuda.d_acumulador1,
				datos_SW_Cuda.d_acumulador2, datos_SW_Cuda.d_deltaTVolumenes, num_volx, num_voly, area, CFL, r, delta_T,
				angulo1, angulo2, angulo3, angulo4, mfc, mf0, mfs, vmax1, vmax2, gravedad, epsilon_h, L, H);

			// Actualizamos el tiempo actual
			tiempo_act += delta_T;

			// Obtenemos el m�nimo delta T aplicando un algoritmo de reducci�n
			dT_min = obtenerMinimoReduccion<float>(datos_SW_Cuda.d_deltaTVolumenes, num_volumenes);

			// Obtenemos el m�nimo delta T de todos los clusters por reducci�n
			MPI_Allreduce (&dT_min, &delta_T, 1, MPI_FLOAT, MPI_MIN, MPI_COMM_WORLD);
//delta_T=5e-4/T;

			// Actualizamos texDatosVolumenes. Dado que los kernels no pueden escribir
			// en texturas, esta copia es inevitable
			cudaMemcpyToArray(datos_SW_Cuda.d_datosVolumenes_1, 0, 1, datos_SW_Cuda.d_acumulador1, tam_datosVolumenes,
				cudaMemcpyDeviceToDevice);
			cudaMemcpyToArray(datos_SW_Cuda.d_datosVolumenes_2, 0, 1, datos_SW_Cuda.d_acumulador2, tam_datosVolumenes,
				cudaMemcpyDeviceToDevice);

			// Inicializamos los acumuladores para la siguiente iteraci�n
			cudaMemset(datos_SW_Cuda.d_acumulador1, 0, tam_datosVolumenes);
			cudaMemset(datos_SW_Cuda.d_acumulador2, 0, tam_datosVolumenes);

			if (id_hebra == 0) {
				fprintf(stdout, "Iteracion %3d, deltaT = %e seg, ", iter, delta_T*T);
				fprintf(stdout, "Tiempo = %g seg\n", tiempo_act*T);
				iter++;
			}
		}
		tiempo_fin = MPI_Wtime();

		// Inicio NetCDF
		if(leer_fichero_puntos == 0) {
		cudaMemcpy(datos_cluster->eta1_maxima, datos_SW_Cuda.d_eta1_maxima, tam_datosEta1, cudaMemcpyDeviceToHost);
		for (j=0; j<ny_nc; j++) {
			pos = (iniy + j*npics)*num_volx;
			for (i=0; i<nx_nc; i++)
				vec[j*nx_nc + i] = (datos_cluster->eta1_maxima[pos + i*npics].x - Hmin)*H;
		}
		closeNC(nx_nc, ny_nc, iniy_nc, vec);
		free(vec);
		}
		// Fin NetCDF

		// Liberamos la memoria de GPU
		liberarSWCuda(&datos_SW_Cuda);
	}
	// Si err == 1, no hay memoria GPU suficiente y la hebra termina
	// (no se puede hacer un return porque estamos en una hebra de MPI)

	*tiempo = tiempo_fin - tiempo_ini;

	return err;
}


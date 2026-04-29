#include <math.h>
#include <fcntl.h>

#include <time.h>

#include <C_General.hpp>
#include <C_Trace.hpp>
#include <C_File.hpp>
#include <C_Arguments.hpp>
#include <C_Matrix.hpp>
#include <C_Image.hpp>	

static void CalcularHistograma(C_Image imagen, int histograma[]) {
	for (int i = imagen.FirstRow(); i <= imagen.LastRow(); i++) {
		for (int j = imagen.FirstCol(); j <= imagen.LastCol(); j++) {
			int valor = (int)imagen(i, j);
			if (valor >= 0 && valor <= 255) {
				histograma[valor]++;
			}
		}
	}
}

void RepresentarHistograma(int histograma[]) {
	for (int i = 0; i <= 255; i++) {
		printf("Nivel %d: %d pixels\n", i, histograma[i]);
	}
	
}

void CalcularHistogramaAcumulado(int histograma[], int histogramaAcumulado[]) {
	histogramaAcumulado[0] = histograma[0];
	for (int i = 1; i <= 255; i++) {
		histogramaAcumulado[i] = histogramaAcumulado[i - 1] + histograma[i];
	}
}

void GenerarKernelGaussiano(C_Matrix & kernel, double sigma) {
	int size = (int)(sigma * 6);
	if (size % 2 == 0) {
		size++;
	}
	int center = size / 2;

	kernel.Resize(0, size - 1, 0, size - 1, 0);

	for (int i = 0; i < size; i++) {
		for (int j = 0; j < size; j++) {
			float x = i - center;
			float y = j - center;
			kernel(i, j) = exp(-(x * x + y * y) / (2 * sigma * sigma)) / (2 * M_PI * sigma * sigma);
		}
	}
}
void GenerarFiltroLineal(C_Image imagen, C_Matrix kernel, int N) {
	C_Image imagenSuavizada(imagen.FirstRow(), imagen.LastRow(), imagen.FirstCol(), imagen.LastCol(), 0);
	int margen = N / 2;

	for (int i = imagenSuavizada.FirstRow() + margen; i <= imagen.LastRow() - margen; i++) {
		for (int j = imagen.FirstCol() + margen; j <= imagen.LastCol() - margen; j++) {
			double valor = 0;
			for (int k = -margen; k <= margen; k++) {
				for (int l = -margen; l <= margen; l++) {
					valor += imagen(i + k, j + l) * kernel(k + margen, l + margen);
				}
			}
			imagenSuavizada(i, j) = (int)valor;
		}
	}
}

void Diferencia_Imagenes(C_Image imagen1, C_Image imagen2, C_Image & resultado) {
	for (int i = resultado.FirstRow(); i <= resultado.LastRow(); i++) {
		for (int j = resultado.FirstCol(); j <= resultado.LastCol(); j++) {
			resultado(i, j) = abs(imagen1(i, j) - imagen2(i, j));
		}
	}
}

int main(){

	//Leer de las imagenes
	char nombreImagen[256];
	char nombreImagen2[256];

	printf("Introduce el nombre de la primera imagen (sin extension): ");
	scanf_s("%s", nombreImagen, sizeof(nombreImagen));

	printf("Introduce el nombre de la segunda imagen (sin extension): ");
	scanf_s("%s", nombreImagen2, sizeof(nombreImagen2));

	const char* rutaBase = "C:/Users/adolf/OneDrive/Desktop/PruebasTDI/";
	char ruta[256];
	char ruta2[256];

	snprintf(ruta, sizeof(ruta), "%s%s.bmp", rutaBase, nombreImagen);
	snprintf(ruta2, sizeof(ruta2), "%s%s.bmp", rutaBase, nombreImagen2);

	C_Image imagen;
	imagen.Read(ruta);

	C_Image imagen2;
	imagen2.Read(ruta2);

	// Histogramas e histogramas acumulados
    int histograma[256] = { 0 };
    CalcularHistograma(imagen, histograma);
    int histogramaAcumulado[256] = { 0 };
    CalcularHistogramaAcumulado(histograma, histogramaAcumulado);

	int histograma2[256] = { 0 };
	CalcularHistograma(imagen2, histograma2);
	int histogramaAcumulado2[256] = { 0 };
	CalcularHistogramaAcumulado(histograma2, histogramaAcumulado2);

	// Establecer Umbrales para el algoritmo de normalización dle histograma
    int totalPixeles = imagen.RowN() * imagen.ColN();
    double umbralBajo = totalPixeles * 0.01;
    double umbralAlto = totalPixeles * 0.99;

    double pBajo = 0;
    for (int i = 0; i <= 255; i++) {
        if (histogramaAcumulado[i] >= umbralBajo) { pBajo = i; break; }
    }
    double pAlto = 0;
    for (int i = 0; i <= 255; i++) {
        if (histogramaAcumulado[i] >= umbralAlto) { pAlto = i; break; }
    }

    for (int i = imagen.FirstRow(); i <= imagen.LastRow(); i++) {
        for (int j = imagen.FirstCol(); j <= imagen.LastCol(); j++) {
            double valor = (double)(imagen(i, j) - pBajo) * 255.0 / (double)(pAlto - pBajo);
            if (valor < 0) valor = 0;
            if (valor > 255) valor = 255;
            imagen(i, j) = (int)valor;
        }
    }

	for (int i = imagen2.FirstRow(); i <= imagen2.LastRow(); i++) {
		for (int j = imagen2.FirstCol(); j <= imagen2.LastCol(); j++) {
			double valor = (double)(imagen2(i, j) - pBajo) * 255.0 / (double)(pAlto - pBajo);
			if (valor < 0) valor = 0;
			if (valor > 255) valor = 255;
			imagen2(i, j) = (int)valor;
		}
	}

	// Suavizar con filtro gaussiano
	double sigma = 1.5;
    C_Matrix kernel;
    GenerarKernelGaussiano(kernel, sigma);
    GenerarFiltroLineal(imagen, kernel, 7);
	GenerarFiltroLineal(imagen2, kernel, 7);

	//Escribir imagenes preprocesada
	double porcentaje = 100 - (umbralAlto / totalPixeles * 100.0);
	snprintf(ruta, sizeof(ruta), "% s % sNormalizado(% .0f % %)Suavizado(% .1f).bmp", rutaBase, nombreImagen, porcentaje, sigma);
	imagen.Write(ruta);
	snprintf(ruta2, sizeof(ruta2), "% s % sNormalizado(% .0f % %)Suavizado(% .1f).bmp", rutaBase, nombreImagen2, porcentaje, sigma);
	imagen2.Write(ruta2);
    
	//Restar imágenes
	C_Image resultado(imagen.FirstRow(), imagen.LastRow(), imagen.FirstCol(), imagen.LastCol(), 0);
	Diferencia_Imagenes(imagen, imagen2, resultado);
	
	if (resultado.Max() == 0) {
		printf("No hay diferencia entre las imagenes\n");
	}
	else
		{
		printf("Las imagenes son diferentes");
	}
	return 0;
}
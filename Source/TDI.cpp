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
			histograma[(int)imagen(i, j)]++;
		}
	}
}

void RepresentarHistograma(int histograma[]) {
	for (int i = 0; i <= 255; i++) {
		printf("Nivel %d: %d pixels\n", i, histograma[i]);
	}
	
}

void CalcularHistogramaAcumulado(int histograma[], int histogramaAcumulado[]) {
	for (int i = 1; i <= 255; i++) {
		histogramaAcumulado[i] = histogramaAcumulado[i - 1] + histograma[i];
	}
}



int main()
{
	//Lectura e inicialización de la imagen

	C_Image imagen;

	imagen.Read("C:/Users/adolf/OneDrive/Desktop/PruebasTDI/CSgris.bmp");

	//Normalización con algoritmo de min y max con recorte de percentiles 
	//Calcular el histograma e histograma acumulado de la imagen

	int histograma[256] = { 0 };
	
	CalcularHistograma(imagen, histograma);

	int histogramaAcumulado[256] = { 0 };

	CalcularHistogramaAcumulado(histograma, histogramaAcumulado);

	//Inicializar las variables y establecer los umbrales

	int totalPixeles = imagen.LastRow() * imagen.LastCol();

	double umbralBajo = totalPixeles * 0.05;

	double umbralAlto = totalPixeles * 0.98;
	
	double pBajo = 0;

	for (int i = 0; i <= 255; i++) {
		if (histogramaAcumulado[i] >= umbralBajo) {
			pBajo = i;
			break;
		}
	}

	double pAlto = 0;

	for (int i = 0; i <= 255; i++) {
		if (histogramaAcumulado[i] >= umbralAlto) {
			pAlto = i;
			break;
		}
	}
	
	//Aplicar algortimo min y max

	C_Image imagenNormalizada(imagen.FirstRow(), imagen.LastRow(),imagen.FirstCol(), imagen.LastCol());

	for (int i = imagen.FirstRow(); i <= imagenNormalizada.LastRow(); i++) {
		for (int j = imagen.FirstCol(); j <= imagenNormalizada.LastCol(); j++) {
			double valor = (double)(imagen(i, j) - pBajo) * 255.0 / (double)(pAlto - pBajo);
			if (valor < 0) valor = 0;
			if (valor > 255) valor = 255;
			imagenNormalizada(i, j) = (int)valor;
		}
	}
	
	double porcentaje = 100 - ((umbralAlto) / totalPixeles * 100.0);
	char ruta[256];
	snprintf(ruta, sizeof(ruta), "C:/Users/adolf/OneDrive/Desktop/PruebasTDI/CSgrisNormalizado(%.0f%%).bmp", porcentaje);
	imagenNormalizada.Write(ruta);
	
}

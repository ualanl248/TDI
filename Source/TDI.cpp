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
	C_Image imagen;

	imagen.Read("C:/Users/adolf/OneDrive/Desktop/PruebasTDI/CSgris.bmp");

	int totalPixeles = imagen.LastRow() * imagen.LastCol();

	double umbralBajo = totalPixeles * 0.95;

	double umbralAlto = totalPixeles * 0.05;

	int histograma[256] = { 0 };
	
	CalcularHistograma(imagen, histograma);

	int histogramaAcumulado[256] = { 0 };

	CalcularHistogramaAcumulado(histograma, histogramaAcumulado);

	RepresentarHistograma(histogramaAcumulado);

	

	//imagen.Write("C:/Users/adolf/OneDrive/Desktop/PruebasTDI/CSgris.bmp");
	
}

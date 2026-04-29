Proyecto para la detección básica de personas o animales en imágenes a partir del número píxeles que ocupan en las mismas en base a su diferencia con una imagen de referencia.

Para ello, antes se sigue un preprocesamiento: En primer lugar se  realiza un estiramiento del histograma con un algoritmo de normalización con percentiles para, posteriormente,
realizar un suavizado mediante un filtro lineal implementado con un kernel gaussiano.

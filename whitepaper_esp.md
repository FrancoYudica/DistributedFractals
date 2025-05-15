# Renderizado de fractales con MPI

Franco Yudica, Martín Farrés

## Abstract

Este trabajo presenta el desarrollo de una implementación paralela de renderizado de fractales bidimensionales, mostrando sus diferencias con la versión secuencial. Además se detalla sobre los experimentos realizados para determinar el rendimiento de la aplicación, en conjutno con conclusiones sobre tales resultados.

## Índice

- [Introducción](#introducción)
- [Marco teórico](#marco-teórico)
  - [Dinámica compleja](#dinámica-compleja)
  - [Conjuntos de Julia y Fatou](#conjuntos-de-julia-y-fatou)
  - [Fractal Julia](#fracal-julia)
  - [Fractal Mandelbrot](#fracal-mandelbrot)
  - [Coloreo de fractales](#coloreo-de-fractales)
- [Desarrollo](#desarrollo)
- [Bibliografía](#bibliografía)

## Introducción

El renderizado eficiente de imágenes fractales representa un reto significativo en el ámbito de la computación gráfica, debido a la complejidad matemática involucrada y la alta demanda computacional que implica su visualización detallada. Los fractales, como el conjunto de Mandelbrot o los conjuntos de Julia, se caracterizan por su estructura auto-similar e infinita complejidad, lo que requiere un gran número de cálculos por píxel para generar imágenes precisas y atractivas.

Este informe presenta el desarrollo y análisis de un sistema de renderizado de fractales, implementado utilizando técnicas de paralelización y optimización computacional. El objetivo principal es reducir el tiempo de renderizado, aprovechando al máximo los recursos de hardware disponibles.

En primer lugar, se describe el proceso secuencial de renderizado de fractales, abordando las ecuaciones iterativas involucradas y los criterios de escape utilizados para determinar la convergencia o divergencia de cada punto. Luego, se introduce la versión paralela del sistema, explicando las estrategias utilizadas para distribuir el trabajo entre múltiples hilos o nodos, y las optimizaciones aplicadas para mejorar la eficiencia.

A continuación, se presentan los experimentos realizados para evaluar el rendimiento del sistema, analizando métricas como el speedup, la eficiencia y la escalabilidad en diferentes configuraciones. Se incluyen también comparaciones visuales y tiempos de renderizado, acompañados de una discusión sobre los resultados.

Finalmente, se exponen las conclusiones obtenidas a partir del análisis, resaltando las ventajas y limitaciones del enfoque propuesto. Se sugieren además posibles líneas de mejora, incluyendo la exploración de técnicas avanzadas como el uso de GPUs, algoritmos adaptativos de muestreo y representación dinámica en tiempo real.

## Marco teórico

### Dinámica compleja

La dinámica compleja, también conocida como dinámica holomorfa [\[7\]](#complex-dynamics), es una rama de la matemática que estudia el comportamiento de los sistemas dinámicos [\[8\]](#dynamic-systems) obtenidos mediante la iteración de funciones analíticas en el plano complejo. A diferencia de los sistemas dinámicos en el plano real, la estructura adicional que proporciona la analiticidad en los números complejos introduce una rica variedad de comportamientos geométricos y topológicos que han sido ampliamente estudiados desde principios del siglo XX.

Una función compleja $f:C→C$ se itera generando una secuencia de funciones

```math
f^n(z)=f(f^{n-1}(z))
```

donde $n \in \mathbb{N}$. El objeto de estudio principal es el conjunto de órbita de un punto $z$, definido como la secuencia de sus imágenes sucesivas bajo iteraciones de $f$. Esta órbita puede exhibir distintos comportamientos: puede tender al infinito, converger a un punto fijo o seguir una trayectoria caótica.

Un ejemplo clásico y fundamental es la función cuadrática:

```math
    f(z) = z²
```

Aunque simple, esta función exhibe una variedad de comportamientos interesantes dependiendo del punto de partida $z_0$

Por ejemplo:

```math
    Si |z_0| < 1, entonces f^n(z_0) → 0
```

```math
    Si |z_0| > 1, entonces f^n(z_0) → ∞
```

```math
    Si |z_0| = 1, entonces f^n(z_0) → 1
```

### Conjuntos de Julia y Fatou

El estudio de estas órbitas lleva a la clasificación del plano complejo en dos regiones fundamentales:

- El conjunto de Fatou, donde las órbitas tienen un comportamiento estable bajo pequeñas perturbaciones iniciales.
- El conjunto de Julia, que contiene puntos con un comportamiento altamente sensible a las condiciones iniciales, caractarizado por su complejiad fractal.

Estos conjuntos son complementarios y su frontera compartida representa el límite entre estabilidad y caos. En el caso de $f(z) = z²$, el conjunto de julia es el círculo unitario $|z|=1$, mientras que el conjunto de Fatou está formado por el interior y el exterior de tal círculo.

### Fracal Julia

Los conjuntos de Julia se generan utilizando números complejos. Estos poseen dos componentes, real e imaginaria, y pueden representarse como puntos en un plano bidimensional, lo que permite renderizar el fractal sobre una imagen 2D. Para cada píxel de la imagen, su coordenada $(x,y)$ en el plano se utiliza como entrada en una función recursiva.

El fractal de Julia es un ejemplo clásico de fractal de tiempo de escape, lo que significa que el interés está en determinar si, tras aplicar repetidamente una función compleja, el valor resultante tiende al infinito o no.

La función recursiva que define el conjunto de Julia es:

```math
z_{n+1}=z_n²+c
```

donde:

- $z_n$​ es el valor complejo en la iteración $n$,
- $z_0$​ es la posición del píxel en el plano complejo, escrita como $z_0=p_x+p_yi$
- $c$ es una constante compleja, $c=c_x+c_yi$, que permanece fija durante toda la generación del fractal.

Está demostrado que si $∣z_n∣>2$, entonces la sucesión diverge (tiende a infinito). En este contexto, el valor 2 se denomina bailout, y es el umbral utilizado para determinar la divergencia. [\[5\]](#fractal-rendering).

### Fracal Mandelbrot

El fractal de Mandelbrot es muy similar al de Julia, ya que también se trata de un fractal de tiempo de escape. La principal diferencia radica en la función recursiva y en los valores iniciales utilizados.

La función que define al conjunto de Mandelbrot es:

```math
z_{n+1}=z_n²+p
```

donde:

- $p$​ es la posición del píxel en el plano complejo, de la forma $p=p_x+p_yi$,

- $z_n$​ inicia en 0, es decir, $z_0=0$, y se itera añadiendo el valor constante $p$​ en cada paso.

Al igual que en el caso del fractal de Julia, el criterio de escape se basa en si $|z_n∣>2$, utilizando el mismo valor de bailout.
[\[2\]](#mandelbrot), [\[5\]](#fractal-rendering).

### Coloreo de fractales

Existen distintos métodos para colorear fractales, siendo el más básico el blanco y negro. En este esquema, los píxeles cuya posición, al ser utilizada como punto de partida en la iteración del fractal, tienden al infinito, se colorean de blanco. Por el contrario, aquellos que no divergen se colorean de negro.

<p align="center">
  <img src="https://github.com/user-attachments/assets/2a7f7217-5f68-4112-9e7c-d65229a3cf2a" width="50%">
</p>

<p align="center"><b>Figura 1:</b> Representación en blanco y negro del conjunto de Mandelbrot.</p>

Sin embargo, este método binario puede resultar limitado para visualizar la complejidad del sistema dinámico. Por ello, se utilizan técnicas más avanzadas como el coloreo por tiempo de escape (escape time coloring), donde se asignan colores según la cantidad de iteraciones que tarda un punto en escapar de un cierto radio. Esto permite generar imágenes con ricos gradientes de color que reflejan la velocidad de divergencia y destacan la estructura del borde del conjunto. [\[5\]](#fractal-rendering).

<p align="center">
  <img src="https://github.com/user-attachments/assets/345a943e-b06d-4152-b39c-9befd2f7ffa9" width="50%">
</p>

<p align="center"><b>Figura 2:</b> Representación en escala de grises del conjunto de Mandelbrot.</p>

Pero también es posible mapear el número de iteraciones a una paleta de colores. Nótese que los puntos pertenecientes al conjunto de Mandelbrot toman un color uniforme, ya que alcanzan el número máximo de iteraciones sin divergir.

<p align="center">
  <img src="https://github.com/user-attachments/assets/3a68cdb6-6754-487f-b32a-5d59af1536e5" width="50%">
</p>

<p align="center"><b>Figura 3:</b> Mapeo de iteraciones a paletea de colores del conjunto de Mandelbrot.</p>

Es interesante observar que se produce un efecto de banding, claramente visible en la Figura 3, ya que los puntos divergentes con igual número de iteraciones toman el mismo color.

## Desarrollo

El algoritmo para el desarrollo de dichas imagenes se resume en, la obtencion del color correspondiente segun la formula de fractal aplicada a cada pixel de la imagen. Para la cual se desarrollo una funcion de renderizado **render_block**;

```plaintext
FUNCION render_block(buffer, config_fractal)

    Obtener funcion_fractal desde config_fractal
    Obtener funcion_color desde config_fractal

    PARA cada fila j desde 0 hasta alto
        PARA cada columna i desde 0 hasta ancho

            pixel_x = x_inicial + i
            pixel_y = y_inicial + j

            Inicializar r, g, b en 0

            PARA cada muestra de antialiasing
                Generar un pequeño desplazamiento aleatorio en x y y

                Calcular coordenadas normalizadas nx, ny

                Convertir (nx, ny) a coordenadas del mundo (wx, wy) usando la camara

                t = funcion_fractal(wx, wy)

                (sample_r, sample_g, sample_b) = funcion_color(t)

                Acumular los valores en r, g, b
            FIN PARA

            Promediar r, g, b y convertir a valores entre 0 y 255

            Guardar el color en el buffer
        FIN PARA
    FIN PARA

FIN FUNCION
```

La funcion se encarga del procesamiento de la imagen fractal. Para cada píxel, el algoritmo toma varias muestras con un pequeño desplazamiento aleatorio (antialiasing) para disminuir el ruído obtenido en la imagen final. Luego cada muestra se transforma en coordenadas del mundo con una cámara virtual. Las mismas son evaluadas por **funcion_fractal** para obtener un valor que, luego es transformados a valores rgb utilizando la funcion, **funcion_color**. Finalmente, dichos valores se promedian obteniendo así el color para cada pixel de la imagen.

Para el desarrollo del problema se hicieron dos versiones, secuencial y paralalelo, para observar las diferencias de ambas en terminos de tiempo y costo computacional.

### Secuencial

El codigo secuencial desarrollado toma un acercamiento lineal al problema. El algoritmo espera ciertos paremetros de configuracion tales como, alto y ancho de la imagen, posicion de la camara y zoom de la misma, que tipo de calculo fractal a utilizar, entre otros. El mismo invoca a la funcion de renderizado para luego guardar la imagen resultante.

### Paralelo

La solución paralela aprovecha la independencia de cada bloque de píxeles para ejecutar el renderizado de forma concurrente.Se adopta un esquema maestro–trabajador: el proceso maestro divide la imagen en bloques, asigna una division equitativa del cómputo, coordina las solicitudes de trabajo de los procesos esclavos y ensambla los resultados parciales en el búfer central.

Esta aproximación inicial ya demuestra mejoras en el tiempo total de cómputo, aunque revela un desbalanceo de carga cuando algunos bloques requieren más cómputo que otros, dejando procesos inactivos mientras otros siguen trabajando.

- Tareas: 1-8
- Nodo1: 0, 1, 2, 3 -> 0-1-2-3
- Nodo2: 4, 5, 6, 7 -> 4----5----6---7

Para resolver este desbalanceo se implementó un balanceo de carga dinámico basado en demanda. En lugar de asignar bloques estáticamente, el maestro mantiene una cola de tareas y cada trabajador solicita un nuevo bloque tan pronto como finaliza el anterior. De este modo, el tiempo de inactividad de los procesos se reduce significativamente y se optimiza el uso de los recursos de cómputo.

- Tareas: 1-8
- Nodo1: -> 0-1-2-3---7
- Nodo2: -> 4----5----6

## Pseudocódigo de `master`

```plaintext
FUNCION master(num_procs, settings)

    CREAR buffer de imagen

    DIVIDIR imagen en tareas (bloques) → lista worker_tasks

    sent ← 0
    done ← 0

    MIENTRAS done < número_de_tareas
        ESPERAR mensaje MPI

        SI mensaje == REQUEST ENTONCES
            SI hay tareas sin enviar ENTONCES
                ENVIAR siguiente tarea
                sent ← sent + 1
            SINO
                ENVIAR TERMINATE
            FIN SI

        SINO SI mensaje == RESULT ENTONCES
            RECIBIR bloque procesado
            COPIAR bloque en buffer principal
            done ← done + 1
        FIN SI
    FIN MIENTRAS

    ENVIAR TERMINATE a todos los workers

    DETENER temporizador e IMPRIMIR duración

    GUARDAR buffer como imagen final

FIN FUNCION
```

La función master comienza reservando un búfer para la imagen completa y dividiendo el área de renderizado en bloques de tamaño fijo, que se almacenan en una lista de tareas. A continuación, mantiene dos contadores: uno para las tareas enviadas y otro para las tareas completadas. En un bucle principal, espera mensajes de los trabajadores; cuando recibe una petición de trabajo, comprueba si aún quedan bloques sin asignar y, en caso afirmativo, envía el siguiente bloque, o bien envía una señal de terminación si ya no hay más. Cuando recibe el resultado de un bloque, copia los píxeles de ese fragmento en la posición correspondiente del búfer global y actualiza el contador de tareas completadas. Este proceso se repite hasta que todas las tareas han sido procesadas, momento en el cual el maestro envía una señal de terminación a cada trabajador, detiene el temporizador y muestra el tiempo total de cómputo. Finalmente, invoca al manejador de salida para guardar el búfer como imagen.

## Pseudocódigo de `worker`

```plaintext
FUNCION worker(rank, config_imagen, config_fractal, camara)

    MIENTRAS verdadero
        ENVIAR mensaje REQUEST al maestro

        ESPERAR mensaje del maestro con cualquier tag

        SI mensaje.tag == TASK ENTONCES
            RECIBIR tarea (x, y, ancho, alto) del maestro
            CREAR buffer de salida de tamaño (ancho * alto * 3)
            LLAMAR render_block(buffer, config_imagen, config_fractal, camara, x, y, ancho, alto)
            ENVIAR tarea al maestro con tag RESULT
            ENVIAR buffer al maestro con tag RESULT

        SINO SI mensaje.tag == TERMINATE ENTONCES
            RECIBIR señal de terminación
            ROMPER el bucle

        FIN SI
    FIN MIENTRAS

FIN FUNCION
```

La función worker arranca enviando al maestro una petición de tarea y se bloquea hasta recibir una respuesta. Cuando llega una tarea, el trabajador crea un búfer para la sección asignada, invoca render_block para rellenarlo con los píxeles fractales correspondientes y luego devuelve tanto la descripción de la tarea como su contenido al proceso maestro. Este ciclo de petición–procesamiento–envío se repite hasta que el maestro indica la terminación, momento en el cual el trabajador sale del bucle y finaliza su ejecución.

## Bibliografía

**[1]** https://solarianprogrammer.com/2013/02/28/mandelbrot-set-cpp-11/

**[2]** <a id="mandelbrot"></a> https://en.wikipedia.org/wiki/Mandelbrot_set

**[3]** https://youtu.be/FFftmWSzgmk?si=KPTdCiAoU7zeQ5VQ

**[4]** https://youtu.be/LqbZpur38nw?si=QAimXxeVIlmIqf3I

**[5]** <a id="fractal-rendering"></a> https://cglearn.eu/pub/advanced-computer-graphics/fractal-rendering

**[6]** https://www.mpi-forum.org/docs/mpi-4.1/mpi41-report.pdf

**[7]** <a id="complex-dynamics"></a>https://en.wikipedia.org/wiki/Complex_dynamics

**[8]** <a id="dynamic-systems"></a>https://es.wikipedia.org/wiki/Sistema_din%C3%A1mico

#### Proyectos de referencia

**[8]** https://github.com/lucaszm7/Mandel2Us

**[9]** https://github.com/Sudo-Rahman/Fractalium

```

```

```

```

```

```

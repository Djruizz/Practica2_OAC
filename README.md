# Práctica 2 OAC – Snake en Ripes

Juego de la serpiente (Snake) en C para el simulador [Ripes](https://github.com/mortbopet/Ripes) (RISC-V), usando periféricos mapeados en memoria.

## Archivos

- `main.c`: lógica del juego.
- `ripes_system.h`: direcciones de los periféricos generadas por Ripes.

## Periféricos

| Periférico         | Uso                                                                                |
| ------------------ | ---------------------------------------------------------------------------------- |
| LED Matrix (20x20) | Pantalla; cada celda del juego ocupa 2x2 LEDs (tablero de 10x10)                   |
| D-Pad              | Controla la dirección de la serpiente                                              |
| Switches           | El switch 0 inicia/pausa el juego; su valor también siembra el generador aleatorio |

## Cómo ejecutarlo

1. Abre Ripes y crea un proyecto en C con `main.c`.
2. Añade a la pestaña _I/O_ una **LED Matrix** (20x20), un **D-Pad** y **Switches**.
3. Compila y ejecuta.

## Cómo jugar

- Activa el **switch 0** para comenzar.
- Usa el **D-Pad** para mover la serpiente (inicia hacia la derecha).
- Come las manzanas (rojas) para crecer; la serpiente es verde.
- El juego termina al chocar con una pared o contigo mismo.
- Apaga el switch 0 y vuélvelo a encender para reiniciar.

## Configuración

En `main.c`:

- `PIXEL_SIZE`: tamaño de cada celda en LEDs.
- `DELTATIME`: retardo entre movimientos (velocidad).

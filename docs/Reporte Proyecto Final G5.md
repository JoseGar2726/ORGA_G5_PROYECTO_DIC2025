# Informe Técnico Proyecto (G5) 
![enter image description here](https://portal.usac.edu.gt/wp-content/uploads/2022/08/escudo-ByN-USAC-2022.png)

**Grupo 5 conformado por:**
**José Antonio García Roca - 202401166**
**Esmeralda Del Rosario Guillén Veliz - 201901002**
**Nelson Andres Santa Cruz Gil - 202202728**
**Angel Emanuel Rodriguez Corado - 202404856**
**Pablo Daniel Solares Samayoa - 202400044**

---

## Introducción
La presente práctica consiste en el diseño e implementación de una maqueta de casa domotizada, integrando hardware y software mediante el uso de un microcontrolador Arduino como unidad central de control. El sistema permite gestionar de forma centralizada la iluminación por ambientes, un sistema de ventilación automatizado y el acceso motorizado de una puerta, todo operado desde una computadora a través de comunicación serial USB.

El proyecto contempla cinco ambientes principales como lo son: sala, comedor, cocina, baño y habitación,representados mediante LEDs que simulan la iluminación de cada área. Adicionalmente, se incorpora un motor DC para simular un ventilador de techo con control de velocidad por PWM, y un servomotor encargado de la apertura y cierre de la puerta principal. La interacción con el sistema se realiza exclusivamente mediante comandos enviados desde la computadora, permitiendo un control preciso, flexible y escalable.

Como parte fundamental del sistema, se implementa el concepto de escenas luminosas, las cuales permiten ejecutar secuencias predefinidas de iluminación y control de dispositivos según diferentes contextos (por ejemplo, Fiesta, Relajado y Noche). Estas escenas son almacenadas en la memoria EEPROM del microcontrolador, garantizando la persistencia de la configuración incluso después de un reinicio o pérdida de energía. La retroalimentación del estado del sistema se presenta de forma local mediante una pantalla LCD I2C.

El desarrollo de esta práctica permite aplicar conocimientos de sistemas embebidos, automatización residencial, control de actuadores, comunicación serial y manejo de memoria no volátil, fortaleciendo habilidades prácticas en el diseño e implementación de sistemas domóticos funcionales, similares a los utilizados en aplicaciones reales de hogares inteligentes e Internet de las Cosas .

---
## Objetivos

**General:** Diseñar e implementar un sistema domótico funcional basado en Arduino que permita el control centralizado de la iluminación por ambientes, un sistema de ventilación automatizado y una puerta motorizada, mediante comunicación serial desde una computadora, incorporando escenas predefinidas, persistencia de datos en EEPROM y retroalimentación visual en tiempo real.

**Especificos:** 
- Implementar el control individual de la iluminación en cinco ambientes distintos utilizando LEDs y comandos enviados por comunicación serial USB.
- Desarrollar un sistema de ventilación simulado mediante un motor DC con control de velocidad por PWM, incorporando al menos tres niveles de operación.
- Diseñar e implementar escenas luminosas predefinidas con secuencias temporales diferenciadas, almacenadas en la memoria EEPROM del microcontrolador.
-   Garantizar la persistencia del estado del sistema mediante la recuperación automática de la última escena activa al reiniciar el Arduino.
- Mostrar el estado operativo del sistema escena activa, estado de la puerta y modo del ventilador mediante una pantalla LCD I2C.


---

## Marco teorico

### Domótica y automatización residencial

La domótica es el conjunto de tecnologías aplicadas al control y automatización inteligente de una vivienda, con el propósito de mejorar la comodidad, seguridad, eficiencia energética y funcionalidad de los espacios habitacionales. Mediante la integración de hardware y software, los sistemas domóticos permiten la gestión centralizada de dispositivos eléctricos y electrónicos, tales como iluminación, ventilación, accesos y sistemas de monitoreo.

En este proyecto, la automatización residencial se simula a través de una maqueta de casa inteligente, en la cual se representan distintos ambientes controlados de forma centralizada, replicando el comportamiento básico de un hogar domotizado real.

### Sistemas embebidos y microcontroladores

Un sistema embebido es un sistema computacional diseñado para cumplir una función específica dentro de un sistema mayor, generalmente con recursos limitados y operación continua. Estos sistemas combinan hardware y software para controlar procesos físicos en tiempo real.

El microcontrolador Arduino Uno, basado en el ATmega328P, es una plataforma ampliamente utilizada en el desarrollo de sistemas embebidos debido a su arquitectura simple, bajo costo y facilidad de programación. En este proyecto, Arduino funciona como la unidad central de control encargada de interpretar comandos, ejecutar la lógica del sistema domótico, controlar actuadores y gestionar la información almacenada en memoria no volátil.

### Comunicación serial USB

La comunicación serial es un método de transmisión de datos en el que la información se envía de manera secuencial, bit a bit, entre dos dispositivos. Este tipo de comunicación es común en sistemas embebidos por su simplicidad y confiabilidad.

En el sistema desarrollado, la comunicación serial USB permite la interacción directa entre la computadora y el Arduino. A través de comandos en formato texto enviados por el puerto serial, el usuario puede controlar la iluminación, el ventilador, la puerta y las escenas del sistema. Este mecanismo facilita el control, la depuración y la expansión futura del sistema, al no depender de interfaces gráficas complejas ni de comunicación inalámbrica.

### Actuadores en sistemas domóticos

Los actuadores son dispositivos que transforman señales eléctricas de control en acciones físicas. En los sistemas domóticos, los actuadores permiten ejecutar las decisiones del sistema de control sobre el entorno.

En este proyecto se emplean tres tipos de actuadores:

LEDs, que simulan la iluminación de cada ambiente y se controlan mediante salidas digitales del microcontrolador.

Motor DC, cuyo funcionamiento se regula mediante señales PWM.

Servomotor, encargado de la apertura y cierre de la puerta, aprovechando su capacidad de posicionamiento angular preciso.

El uso combinado de estos actuadores permite simular de manera realista el funcionamiento de una vivienda automatizada.

### Modulación por ancho de pulso (PWM)

La modulación por ancho de pulso (PWM) es una técnica que permite controlar la potencia suministrada a un dispositivo eléctrico mediante la variación del tiempo que una señal digital permanece en estado alto dentro de un periodo determinado. Aunque la señal es digital, el efecto resultante es similar al de una señal analógica.

En este sistema, el PWM se utiliza para controlar la velocidad del motor DC que simula el ventilador. Mediante distintos valores de ciclo de trabajo, se definen varios niveles de velocidad, lo que permite un control más preciso y eficiente del actuador. Esta técnica es fundamental en aplicaciones de automatización y control de motores.

### Memoria EEPROM y persistencia del sistema

La memoria EEPROM es un tipo de memoria no volátil integrada en el microcontrolador, capaz de conservar la información almacenada incluso cuando el sistema se queda sin alimentación eléctrica.

En el proyecto, esta se emplea para almacenar las escenas luminosas y la última configuración activa del sistema. Gracias a esta característica, al reiniciar el Arduino el sistema puede recuperar automáticamente su estado previo. La persistencia de datos es un aspecto clave en los sistemas domóticos reales, ya que garantiza continuidad en la operación y conservación de las preferencias del usuario.

### Escenas luminosas y control secuencial

Las escenas luminosas son configuraciones predefinidas que agrupan múltiples acciones de control bajo un solo comando. Estas escenas permiten ejecutar secuencias automáticas de encendido y apagado de dispositivos, ajustadas a distintos contextos o necesidades.


### Pantalla LCD I2C como interfaz de usuario

La interfaz de usuario es un componente esencial en los sistemas embebidos, ya que permite al usuario conocer el estado del sistema de forma clara e inmediata. Las pantallas LCD son ampliamente utilizadas por su bajo consumo y facilidad de integración.

En este proyecto se utiliza una pantalla LCD I2C de 16×2 caracteres, para mostrar la información relevante como la escena activa, el estado de la puerta y el modo de operación del ventilador. El uso del protocolo I2C reduce la cantidad de pines necesarios, optimizando el diseño del circuito y mejorando la organización del sistema.

---

##  Funcionamiento del sistema

### Control central del sistema :

El sistema domótico desarrollado funciona a partir de un microcontrolador Arduino Uno, el cual actúa como unidad central de control. Arduino recibe comandos enviados desde una computadora mediante comunicación serial USB, los interpreta y ejecuta las acciones correspondientes sobre los diferentes módulos del sistema, coordinando el funcionamiento general de la maqueta.

### Comunicación serial USB :

La interacción entre el usuario y el sistema se realiza a través del puerto serial, utilizando comandos de texto. Arduino permanece en escucha constante del canal serial, permitiendo el control de luces, ventilador, puerta y escenas. Asimismo, el sistema envía mensajes de respuesta que confirman la ejecución de cada acción y el estado actual del sistema.

### Iluminación por ambientes:

La iluminación se divide en cinco ambientes representados por LEDs controlados de manera individual mediante salidas digitales del Arduino. Cada ambiente puede encenderse o apagarse mediante comandos seriales, o bien ser gestionado automáticamente como parte de una escena luminosa predefinida.

### Sistema de ventilación:

El sistema de ventilación se simula mediante un motor DC controlado por modulación por ancho de pulso (PWM). A través de distintos valores de PWM, se establecen varios niveles de velocidad, permitiendo regular el funcionamiento del ventilador de forma eficiente desde comandos seriales o escenas automáticas.

### Control de puerta:

La apertura y cierre de la puerta se realiza mediante un servomotor controlado por Arduino. El sistema utiliza una lógica de alternancia, permitiendo cambiar el estado de la puerta con un solo comando. Cada cambio de estado se refleja de manera física y en la retroalimentación del sistema.

### Escenas luminosas y persistencia:

El sistema incorpora escenas luminosas que ejecutan secuencias automáticas de control sobre la iluminación. Estas escenas se almacenan en la memoria EEPROM del microcontrolador, lo que permite conservar la última configuración activa y recuperarla automáticamente al reiniciar el sistema, garantizando persistencia y continuidad operativa.

### Retroalimentación visual:

El estado del sistema se muestra mediante una pantalla LCD I2C, en la cual se visualiza la escena activa, el estado de la puerta y el modo del ventilador. Esta interfaz permite al usuario supervisar el funcionamiento del sistema en tiempo real de manera clara y directa.

### Funcionamiento en vivo:
A continuación se presenta la evidencia gráfica del funcionamiento del sistema de automatización residencial. Las capturas validan la interacción exitosa entre el usuario y el microcontrolador a través del Monitor Serial, mostrando el envío de comandos y la retroalimentación en tiempo real. Asimismo, se documenta la respuesta visual en la pantalla LCD, desplegando información dinámica sobre las escenas activas ('Fiesta', 'Relajado', 'Noche'), la correcta recuperación de datos desde la memoria EEPROM tras el reinicio del sistema.

![Escena1](https://github.com/user-attachments/assets/993bbdf4-5a3d-4118-98fa-2cb38950fd79)
![Escena2](https://github.com/user-attachments/assets/c2fee363-bee7-4756-910e-8dc6d86c2044)
![Escena3](https://github.com/user-attachments/assets/a290f7a8-c8e0-484a-b855-c6ff2ab73340)
![Escena4](https://github.com/user-attachments/assets/edf15bf3-0da5-427c-bb82-194056385413)
![Escena5](https://github.com/user-attachments/assets/2abaded4-c33b-4fb1-a521-09850f00ab9a)

---

##  Diagramas de diseño

### DIAGRAMA TINKERCAD
<img width="1406" height="783" alt="diagrama-inferior" src="https://github.com/user-attachments/assets/b9f7cd6a-ad39-40b1-b167-8dfc90091112" />
<img width="1567" height="808" alt="diagrama-superior" src="https://github.com/user-attachments/assets/86c24188-6684-4bfd-a187-6c188f4d36f0" />
<img width="992" height="764" alt="image" src="https://github.com/user-attachments/assets/4b0c6917-d17a-4d1c-b75e-a6ab4e426e83" />

---


## Materiales utilizados
| Nombre del componente           | Código / Especificación                                                                 |
|--------------------------------|------------------------------------------------------------------------------------------|
| Arduino Uno R3                  | ATmega328P, 16 MHz, 14 pines digitales (6 PWM), 6 entradas analógicas, 5V                |
| Pantalla LCD I2C 16x2           | Display 16×2, interfaz I2C (SDA/SCL), controlador PCF8574, dirección 0x27 / 0x3F, 5V     |
| LEDs (5 unidades)               | LEDs de 5 mm, colores variados, corriente nominal 20 mA, iluminación por ambiente       |
| Resistencias limitadoras        | 220 Ω, 1/4 W, ±5%, protección de corriente para LEDs                                     |
| Motor DC                        | Motor DC 3–6 V, 200–300 RPM, consumo aproximado 100–200 mA (simulación de ventilador)   |
| Driver para motor DC            | L293D o L298N, control de dirección y velocidad mediante PWM                             |
| Servomotor SG90                 | Micro servo 0–180°, 4.8–6 V, torque 1.8 kg/cm, control por señal PWM                     |
| Protoboard                      | 830 puntos, montaje sin soldadura                                                        |
| Botones pulsadores (opcional)   | Pulsadores momentáneos NO, 4 pines, uso lógico o control alterno                          |
| Cable UTP                       | Cables macho–macho y macho–hembra para conexiones del circuito                            |
| Cable USB                       | Tipo A–B, alimentación y comunicación serial con la computadora                          |
| Fuente de alimentación          | Fuente DC regulada 5V/2A o 9V/1A, alimentación estable del sistema                        |
| Material para maqueta           | Cartón, madera balsa, foamboard, MDF o impresión 3D (estructura de la casa)              |





---

##  Presupuesto
Se reutilizaron algunos materiales ,ya que se tenian adquiridos de cursos u otras actividades anteriores ,solamante se adquirio los faltantes.

 Dando un total de materiales faltantes utilizados de Q295 entre los materiales que se usaron para la construcción de la maqueta como para el circuito fisico

---


## Roles de equipo

- Lógica Arduino : José Antonio García Roca
- Diseño Físico Arduino: Esmeralda Del Rosario Guillén Veliz
- Lógica Arduino: Nelson Andres Santa Cruz Gil 
- Diseño Físico Maqueta: Angel Emanuel Rodriguez Corado
- Lógica Arduino : Pablo Daniel Solares Samayoa


---
## Conclusiones

- Se logró diseñar e implementar satisfactoriamente un sistema domótico funcional basado en Arduino, capaz de controlar de forma centralizada la iluminación por ambientes, un sistema de ventilación automatizado y una puerta motorizada mediante comunicación serial desde una computadora. La integración de escenas predefinidas, el uso de memoria EEPROM para la persistencia de datos y la retroalimentación visual en tiempo real a través de una pantalla LCD I2C demuestran que el sistema cumple con los principios fundamentales de automatización residencial y sistemas embebidos, validando el cumplimiento del objetivo general planteado.

- Se implementó correctamente el control individual de la iluminación en cinco ambientes distintos mediante LEDs, los cuales responden de forma inmediata a los comandos enviados por comunicación serial USB. Este resultado evidencia una correcta gestión de las salidas digitales del microcontrolador y una comunicación confiable entre la computadora y el sistema, permitiendo un control preciso y centralizado de cada área de la maqueta.

- Se diseñaron e implementaron escenas luminosas predefinidas con secuencias temporales diferenciadas, las cuales permiten automatizar el comportamiento del sistema según distintos contextos. El almacenamiento de estas escenas en la memoria EEPROM garantiza su disponibilidad incluso después de un reinicio del sistema, evidenciando un manejo adecuado de memoria no volátil y una correcta separación entre lógica de control y configuración del sistema.

- Se garantizó la persistencia del estado del sistema mediante la recuperación automática de la última escena activa al reiniciar el Arduino. Este comportamiento confirma el uso efectivo de la memoria EEPROM y demuestra la importancia de la persistencia de datos en sistemas domóticos, asegurando continuidad operativa y conservación de las preferencias del usuario.

- Se logró mostrar de forma clara y consistente el estado operativo del sistema, incluyendo la escena activa, el estado de la puerta y el modo del ventilador, utilizando una pantalla LCD I2C. Esta retroalimentación visual facilita la supervisión del sistema en tiempo real y mejora la interacción con el usuario, cumpliendo con los requerimientos de interfaz y monitoreo establecidos en el proyecto.

---
## Bibliografía
- https://docs.arduino.cc/
---

---
## ANEXOS
- Link Video:https://youtu.be/lYLnU8sjeug
- Link Simulacion Tinkercad: https://www.tinkercad.com/things/anowxJ8Rz09-proyecto-orga-2025-final?sharecode=5KJGbbnubmVv6DNVDn6xw-GjzaVTudjRTotKl2c_oqE
---

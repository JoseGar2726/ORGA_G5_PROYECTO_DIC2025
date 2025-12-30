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


---

##  Funcionamiento del sistema



---

##  Diagramas de diseño


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
| Cables jumper / UTP             | Cables macho–macho y macho–hembra para conexiones del circuito                            |
| Cable USB                       | Tipo A–B, alimentación y comunicación serial con la computadora                          |
| Fuente de alimentación          | Fuente DC regulada 5V/2A o 9V/1A, alimentación estable del sistema                        |
| Material para maqueta           | Cartón, madera balsa, foamboard, MDF o impresión 3D (estructura de la casa)              |





---

##  Presupuesto
Se reutilizaron la mayoria de materiales ,ya que  se tenian adquiridos de cursos  u otras actividades anteriores solamante se adquirio los faltantes los cuales eran:

 Dando un total de materiales faltantes utilizados de Q295.

---


## Roles de equipo

- : José Antonio García Roca
- : Esmeralda Del Rosario Guillén Veliz
- : Nelson Andres Santa Cruz Gil 
- : Angel Emanuel Rodriguez Corado
- :Pablo Daniel Solares Samayoa


---
## Conclusiones


---
## Bibliografía
---

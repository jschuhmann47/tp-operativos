# TP SO 1C 2022
Trabajo Practico de Sistemas Operativos - 1er Cuatrimestre 2022

Hecho por [@jschuhmann47](https://github.com/jschuhmann47 "@jschuhmann47") y [@JuaaannCruuuzz](https://github.com/JuaaannCruuuzz "@JuaaannCruuuzz").
Sí, por dos personas (se supone que es de a cinco):
- Uno de nosotros laburaba
- Era la primera vez que la cursabamos

Se puede hacer de a 2? Sí, es ir a la guerra pero es posible.

Lo hicimos con VSCode (Eclipse 😠) usando tambien la biblioteca [commons](https://github.com/sisoputnfrba/so-commons-library "commons") de la catedra, Makefiles para compilar y scripts de shell para hacer más rápido el deploy. El entorno de desarrollo y testeo fue Lubuntu 16.04

[El enunciado](https://docs.google.com/document/d/17WP76Vsi6ZrYlpYT8xOPXzLf42rQgtyKsOdVkyL5Jj0/edit# "El enunciado")

## English version
Group assignment made by [@jschuhmann47](https://github.com/jschuhmann47 "@jschuhmann47") and [@JuaaannCruuuzz](https://github.com/JuaaannCruuuzz "@JuaaannCruuuzz") which consisted in making a simulation of a kernel interacting with several hardware components (also simulated) via the C language. 

Architecture of the system is below (spanish):
![image](https://github.com/jschuhmann47/tp-operativos/assets/76596375/8b8043b4-0001-4517-822f-e7b616f17baf)

The four modules were:
- Console: Acts as the user, sending a program to the Kernel. The possible instructions were NOOP (wait for n seconds), READ, WRITE, or COPY in RAM, IO (block for n seconds), and EXIT.
- Kernel: Handles the console connections, selects which program has to execute for the current instruction cycle. Processes can be suspended, blocked, in queue or executing (one at a time). Two algorithms were made: FIFO and SRT (shortest remaining time)
- CPU: recieves instructions and executes them


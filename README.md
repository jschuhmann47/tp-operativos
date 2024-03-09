# TP SO 1C 2022

Group assignment made by [@jschuhmann47](https://github.com/jschuhmann47 "@jschuhmann47") and [@JuaaannCruuuzz](https://github.com/JuaaannCruuuzz "@JuaaannCruuuzz") which consisted in making a simulation of a kernel interacting with several hardware components (which were also simulated) via the C language. Made for the subject "Operating Systems".

Architecture of the system is below (spanish):
![image](https://github.com/jschuhmann47/tp-operativos/assets/76596375/8b8043b4-0001-4517-822f-e7b616f17baf)

The four modules were:
- Console: Acts as the user, sending a program to the Kernel. The possible instructions were NOOP (wait for n seconds), READ, WRITE, or COPY in RAM, IO (block for n seconds), and EXIT.
- Kernel: Handles the console connections, selects which program has to execute for the current instruction cycle. Processes can be suspended, blocked, in queue or executing (one at a time). Two algorithms were made: FIFO and SRT (shortest remaining time)
- CPU: recieves instructions and executes them, calling RAM module if it's an IO instruction, and has translation from logic memory addresses to physical ones via a TLB. It can be interrupted by the Kernel.
- RAM and SWAP: has data stored in blocks that can be read and written. It has a two-level hierarchical pagination layout. The actual RAM is a void* variable at its core, and SWAP are files on disk which can be retrieved later (for example if the process is suspended).

This project was made in approximately three months.


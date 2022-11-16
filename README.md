In this project, we implemented different scheduling algorithms studied in progress: FCFS, RR, SJF, SRTF; and in the last part real-time scheduling algorithms: EDF and RM.
  
This scheduling simulator has several commands. The "make" command allows you to compile the simulator and "make clean" to delete the generated files.
The "make simulate_fcfs" command allows you to simulate the fcfs scheduler. You can replace fcfs with the scheduler of your choice (rr, sjf or srtf). When you run "make simulate_fcfs", a preview.ps file is created. It contains the results of your simulation, you can open it with the "evince preview.ps" command.
Finally, the "make rapport" command allows you to simulate the four schedulers and generate the final rapport.ps file.

> To be able to generate the LaTeX report of the simulator, you must install the packages with the following command:
>  * $ sudo apt-get install --no-install-recommends texlive-fonts-recommended
>  * texlive-latex-base texlive-generic-recommended texlive-pstricks
>  * texlive-latex-extra texlive-lang-french

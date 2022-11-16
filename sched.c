/* Copyright (c) 2013 Pablo Oliveira <pablo.oliveira@prism.uvsq.fr>.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to
 * deal in the Software without restriction, including without limitation the
 * rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
 * sell copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:

 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.

 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
 * IN THE SOFTWARE.  All rights reserved.
 */

#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include "sched.h"

/* --Scheduler random--*/
tproc * randomscheduler(tlist * procs, tlist * ready, int * delta) {
    int length = len(ready);
    int selected = rand()%length;
    tnode * p = ready->first;
    for (int i=0; i < selected; i++) {
        p = p->next;
    }
    *delta = rand()%p->proc->remaining + 1;
    return p->proc;
}
/* --Scheduler random--*/

/* --Scheduler fcfs-- */
tproc * fcfs(tlist * procs, tlist * ready, int * delta) {
    tproc *premier_processus_pret=ready->first->proc; //recuperation de la structure du premier processus pret
    *delta=premier_processus_pret->remaining; //duree d'execution du processus
    return premier_processus_pret; 
}
/* --Scheduler fcfs-- */

/* --Scheduler rr-- */
tproc * rr(tlist * procs, tlist * ready, int * delta) {
    tproc *premier_processus_pret=ready->first->proc;
    static tproc *prioritaire; //static afin de conserver l'etat de prioritaire" entre les appels
    *delta=1; //car le quantum est 1
    if(premier_processus_pret==prioritaire)
    {
    	del(ready,premier_processus_pret);
    	add(ready,premier_processus_pret);
    	prioritaire=ready->first->proc;
    }
    else
    {
    	 prioritaire=ready->first->proc;
    }
    return prioritaire;
}
/* --Scheduler rr-- */

/* --Scheduler sjf-- */
tproc * sjf(tlist * procs, tlist * ready, int * delta) {
    //c'est la version non preemptive qui a ete implementee
    int dureeDexecutionDuProcessusActuel=0;
    int plusCourteDureeDexecution=0;
    tproc *plusCourtProcessus = ready->first->proc;
    tnode *processus=ready->first;
    //detection du processus le plus rapide en terme d'execution
    while(processus!=NULL)
    {
    	dureeDexecutionDuProcessusActuel=processus->proc->remaining;
    	plusCourteDureeDexecution=plusCourtProcessus->remaining;
    	if(dureeDexecutionDuProcessusActuel < plusCourteDureeDexecution)
    	{
    		plusCourtProcessus=processus->proc;
    	}
    	processus=processus->next;
    } 
    plusCourteDureeDexecution=plusCourtProcessus->remaining;
    *delta=plusCourteDureeDexecution; //car non preemptive
    return plusCourtProcessus;
}
/* --Scheduler sjf-- */

/* --Scheduler srtf-- */
tproc * srtf(tlist * procs, tlist * ready, int * delta) {
    //c'est la version preemptive qui a ete implementee
    int dureeDexecutionDuProcessusActuel=0;
    int plusCourteDureeDexecution=0;
    tproc *plusCourtProcessus = ready->first->proc;
    tnode *processus=ready->first;
    //detection du processus le plus rapide en terme d'execution
    while(processus!=NULL)
    {
    	dureeDexecutionDuProcessusActuel=processus->proc->remaining;
    	plusCourteDureeDexecution=plusCourtProcessus->remaining;
    	if(dureeDexecutionDuProcessusActuel < plusCourteDureeDexecution)
    	{
    		plusCourtProcessus=processus->proc;
    	}
    	processus=processus->next;
    } 
    *delta=1;
    return plusCourtProcessus;
}
/* --Scheduler srtf-- */

/* --Scheduler rm-- */
tproc * rm(tlist * procs, tlist * ready, int * delta) {
   tnode *processus = ready->first;
   tproc *leTravailLePlusProprietaire = ready->first->proc;
   while(processus!=NULL )
   {	
	if (processus->proc->period < leTravailLePlusProprietaire->period) leTravailLePlusProprietaire = processus->proc; // car la pririorite d une tache i est 1 divise par Ti c est a dire plus la periode est petite plus la tache est prioritaire
	processus=processus->next;
   }
   *delta = 1;
   return leTravailLePlusProprietaire;
}

/* --Scheduler edf-- */
tproc * edf(tlist * procs, tlist * ready, int * delta) {
   tnode *processus = ready->first;
   tproc *leTravailLePlusProprietaire = ready->first->proc;
   while(processus!=NULL)
   {
    	if((processus->proc->period + processus->proc->activation) < (leTravailLePlusProprietaire->period + leTravailLePlusProprietaire->activation)) leTravailLePlusProprietaire = processus->proc; //car le travail le plus prioritaire est celui dont l echeance est la plus courte
    	processus=processus->next;
   } 
   *delta=1;
   return leTravailLePlusProprietaire;
}

/* List of ready procs */
tlist ready;

/* List of other procs */
tlist procs;

/* The selected scheduler */
tscheduler scheduler;

/* The scheduling statistics */
tstats stats = {0} ;

/* display usage message */
void usage() {
    fail("usage: sched [fcfs, rr, sjf, srtf, edf, rm]\n");
}

/* simulate a single core scheduler, from time 0 to `max_time` */
void simulate(int max_time) {
    int time=0;
    while(time <= max_time) {
        /* Activate process */
        for (tnode * p = procs.first; p != NULL;) {
            tproc * proc = p->proc; 
            p = p->next;

            /* Move every process which should be activated,
             * from the procs list to the ready list */
            if (proc->activation <= time) {
                del(&procs, proc);
                add(&ready, proc);
            }
        }

        /* If any process is ready, then we can schedule! */
        if (ready.first != NULL) {

            int delta = 0;
            /* Call the scheduler */
            tproc * proc = scheduler(&procs, &ready, &delta);

            /* Ensure the scheduler has advanced at least one unit of time */
            assert(delta > 0);
 
 	    //condition à respecter: temps restant à exécuter == durée de l'éxecution du proc
	    // afin de déterminer le temps de reponse, on fait le total de time - temps de l'activation du processus
	    if (proc->length==proc->remaining ) stats.response = stats.response + (time - proc->activation);
			
            /* Output task execution */
            if(proc->activation + proc->period <= time && proc->period)
            {
            	 printf("\\TaskExecution[color=red]{%d}{%d}{%d}\n", proc->pid, time, time+delta);
            }
	    else
	    {
	    	 printf("\\TaskExecution{%d}{%d}{%d}\n", proc->pid, time, time+delta);
	    }
	  

            /* Advance time by delta */
            time += delta;

            /* Remove delta from chosen process */
            proc->remaining-=delta;
 
            /* If the process remaining time is less zero or less, 
             * delete it */ 
            if (proc->remaining <= 0) {
                del(&ready, proc);
                del(&procs, proc);
                
                // afin de déterminer le temps de completion, pour chaque processus, on fait le total temps de fin d'execution - temps d'activation du processus 
                stats.completion = stats.completion + (time - proc->activation);
                
                // afin de déterminer le temps d'attente, on fait le total de (time - temps d'activation du processus - son durée d'exécution) 
                stats.waiting = stats.waiting + (time - proc->activation - proc->length );
                
                // gestion des taches périodiques
                if(proc->period != 0)
                {
                	// update de "remaining"  et de "activation"
                	proc->activation = proc->activation + proc->period;
                	proc->remaining = proc->length;
                	// ajout de la tache dans la liste des taches
                	add(&procs, proc);
                }
            }
        } 
        /* If no process is ready, just advance the simulation timer */
        else {
            time += 1;
        }

    }
}

int main(int argc, char * argv[]) {

    /* Parse arguments */
    if (argc != 2) usage(); 

    /* Seed random number generator */
    srand(time(NULL) ^ getpid());

    char * method = argv[1]; 

    /* The sched argument should be one of fcfs, rr, sjf, srtf */
    if (strcmp(method, "fcfs") == 0) {
        scheduler = fcfs;
    } 
    else if (strcmp(method, "rr") == 0) {
        scheduler = rr;
    }
    else if (strcmp(method, "sjf") == 0) {
        scheduler = sjf;
    }
    else if (strcmp(method, "srtf") == 0) {
        scheduler = srtf;
    }
    else if(strcmp(method, "edf") == 0)
    {
    	 scheduler = edf;
    }
    else if(strcmp(method, "rm") == 0)
    {
    	 scheduler = rm;
    }
    else {
        usage();
    }

    /* Compile the task descriptions */
    #include "task_description.h"

    /* Add all tasks to the procs queue */
    for(int i = 0; i < sizeof(tasks)/sizeof(tproc); i ++) {
        add(&procs, &(tasks[i]));
    }
    
    /* Output RTGrid header */
    printf("\\begin{RTGrid}[width=0.8\\textwidth]{%d}{%d}\n", len(&procs), max_time);

    /* Output task arrivals for all tasks */ 
    for (tnode * p = procs.first; p != NULL; p = p->next) {
        printf("\\TaskArrival{%d}{%d}\n", p->proc->pid, p->proc->activation); 
    }

    /* Start scheduling simulation */
    simulate(max_time);

    /* Close RTGrid environment */
    printf("\\end{RTGrid}\\newline\\newline\n");

    /* Print statistics */
    printf("Total completion time = %d\\newline\n", stats.completion);
    printf("Total waiting time = %d\\newline\n", stats.waiting);
    printf("Total response time = %d\\newline\n", stats.response);

    /* Empty the lists if needed */
    del_all(&ready);
    del_all(&procs);

    return 0;
}

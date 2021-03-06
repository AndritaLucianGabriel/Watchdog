#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
int main()
{

	system("clear");
	//lista pentru procese de verificat
	FILE *f=fopen("./Proiect_Watchdog/lista.txt","r");	
	//fisierul registru.txt va tine evidenta schimbarilor facute
	FILE *g=fopen("./Proiect_Watchdog/registru.txt", "a");
	//timestamp al executarii aplicatiei
	time_t t = time(NULL);
  	struct tm tm = *localtime(&t);
 	fprintf(g,"\n\n\n\t\tInstanta rulata la: %d-%02d-%02d %02d:%02d:%02d\n\n\n\n", tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec);

        int *v,*limita_res,dim=0;
        char **limita_cpu,**limita_mem;
	v=(int *)malloc((dim+1)*sizeof(int));
	limita_res=(int *)malloc((dim+1)*sizeof(int));
	limita_cpu=(char**)malloc((dim+1)*sizeof(char*));
		limita_cpu[dim]=(char*)malloc(4*sizeof(char));
	limita_mem=(char**)malloc((dim+1)*sizeof(char*));
                limita_mem[dim]=(char*)malloc(4*sizeof(char));
	while(feof(f)==NULL)
	{
		fscanf(f,"%d %d %4s %4s", &v[dim], &limita_res[dim], limita_cpu[dim],limita_mem[dim]);
		dim++;
		v=realloc(v,(dim+1)*sizeof(int));
		limita_res=realloc(limita_res,(dim+1)*sizeof(int));
	        limita_cpu=(char**)realloc(limita_cpu,(dim+1)*sizeof(char*));
                	limita_cpu[dim]=(char*)malloc(4*sizeof(char));	
		limita_mem=(char**)realloc(limita_mem,(dim+1)*sizeof(char*));
                        limita_mem[dim]=(char*)malloc(4*sizeof(char));
	}
	free(limita_cpu[dim]);
	free(limita_mem[dim]);
	dim--;
	v=realloc(v,dim*sizeof(int));	
	limita_res=realloc(limita_res,dim*sizeof(int));
	limita_cpu=realloc(limita_cpu,dim*sizeof(char*));
	limita_cpu=realloc(limita_cpu,dim*sizeof(char*));
	
	//verificare citire date
	fprintf(g,"In lista.txt avem informatii despre %d PIDuri.\n\n", dim);
	for(int i=0;i<dim;i++)
		fprintf(g,"Avem urmatoarele constrangeri pentru PID=%d:\nlimita_res=%dkb\nSa nu fie zombi\nlimita_CPU=%s%\nlimita_MEM=%s%\n\n",v[i],limita_res[i],limita_cpu[i],limita_mem[i]);

	//apelare functie pentru informatii despre procese
	system("top -b -n 1 > ./Proiect_Watchdog/Fisiere_suport/procese.txt");	
	system("wc -l ./Proiect_Watchdog/Fisiere_suport/procese.txt > ./Proiect_Watchdog/Fisiere_suport/LUNG.txt");//numar randuri
	FILE *lung=fopen("./Proiect_Watchdog/Fisiere_suport/LUNG.txt","r");
	int LUNG;
	fscanf(lung,"%d", &LUNG);
	fprintf(g,"Fisierul procese.txt are %d linii.\n\n", LUNG);//retinerea numarului de randuri in LUNG

	//populare fisiere individuale
	system ("awk '{print $1}' ./Proiect_Watchdog/Fisiere_suport/procese.txt > ./Proiect_Watchdog/Fisiere_suport/PID.txt");
	system ("awk '{print $6}' ./Proiect_Watchdog/Fisiere_suport/procese.txt > ./Proiect_Watchdog/Fisiere_suport/RES.txt");
	system ("awk '{print $8}' ./Proiect_Watchdog/Fisiere_suport/procese.txt > ./Proiect_Watchdog/Fisiere_suport/STARE.txt");
	system ("awk '{print $9}' ./Proiect_Watchdog/Fisiere_suport/procese.txt > ./Proiect_Watchdog/Fisiere_suport/CPU.txt");
	system ("awk '{print $10}' ./Proiect_Watchdog/Fisiere_suport/procese.txt > ./Proiect_Watchdog/Fisiere_suport/MEM.txt");
	
	
	//deschidere fisiere cu informatii
	FILE *pid=fopen("./Proiect_Watchdog/Fisiere_suport/PID.txt","r");
	FILE *res=fopen("./Proiect_Watchdog/Fisiere_suport/RES.txt","r");
        FILE *stare=fopen("./Proiect_Watchdog/Fisiere_suport/STARE.txt","r");
        FILE *mem=fopen("./Proiect_Watchdog/Fisiere_suport/MEM.txt","r");
        FILE *cpu=fopen("./Proiect_Watchdog/Fisiere_suport/CPU.txt","r");

	//cursorul ajunge la informatii in toate fisierele
	char *merge;
	merge=(char *)malloc(50*sizeof(char));
	for(int i=0;i<6;i++)
	{
		fscanf(pid, "%50s", merge);
		fscanf(res, "%50s", merge);
 		fscanf(stare, "%50s", merge);
		fscanf(mem, "%50s", merge);
 		fscanf(cpu, "%50s", merge);	
	}
	
	//citirea din fisiere a valorilor de tip float este interpretata sub forma de sir de caractere
	int *PID,*RES;//vectori ce stocheaza informatia
	char **STARE,**MEM,**CPU;//matrici de caractere

	//alocari memorie
	PID=(int *)calloc(dim,sizeof(int));
        RES=(int *)malloc(dim*sizeof(int));
        
	STARE=(char **)malloc(dim*sizeof(char*));
	for(int i=0;i<dim;i++)
		STARE[i]=(char*)malloc(2*sizeof(char));
        MEM=(char **)malloc(dim*sizeof(char*));
       	for(int i=0;i<dim;i++)
                MEM[i]=(char*)malloc(4*sizeof(char));

        CPU=(char **)malloc(dim*sizeof(char*));
        for(int i=0;i<dim;i++)
                CPU[i]=(char*)malloc(4*sizeof(char));

	//valori locale pentru citirea informatiilor
        int local_pid, local_res;
	char *local_mem,*local_cpu,*local_stare;	
	local_mem=(char*)malloc(4*sizeof(char));
	local_cpu=(char*)malloc(4*sizeof(char));
	local_stare=(char*)malloc(2*sizeof(char));

	int i,j,contor=0;
	int *v_frecventa=(int *)calloc(dim,sizeof(int));//vector ce tine evidenta aparitiei pid urilor din lista data in informatiile oferite de top();	

	for(i=0;i<LUNG-7;i++)//citim si populam vectorul pid-urilor pe baza aparitiei
	{		
		fscanf(pid, "%d", &local_pid);
                fscanf(res, "%d", &local_res);
                fscanf(stare, "%s", local_stare);
                fscanf(mem, "%4s", local_mem);
                fscanf(cpu, "%4s", local_cpu);	
		
		for(j=0;j<dim;j++)
		{

			if(local_pid==v[j])//daca pid-ul este in lista propusa
			{
				//stocam informatia
				PID[contor]=local_pid;
				RES[contor]=local_res;
				strcpy(STARE[contor],local_stare);
				strcpy(MEM[contor],local_mem);
				strcpy(CPU[contor],local_cpu);
				contor++;
				v_frecventa[j]++;//contorizam ce pid nu a fost gasit
				break;
			}
		}

	}	
	
	//afisam informatia
	fprintf(g,"Avem urmatoarele informatii despre procesele din lista: \n\n");
	for(i=0;i<dim;i++)
	{
		if(PID[i]!=0)
			fprintf(g,"PID=%d RES=%d STARE=%s CPU=%s MEM=%s\n", PID[i],RES[i],STARE[i],CPU[i],MEM[i]);
		else
			for(j=0;j<dim;j++)
				if(v_frecventa[j]==0)
				{
					v_frecventa[j]++;
					fprintf(g,"\nNu exista procesul cu PID = %d din lista data. \n", v[j]);
				}
	}
	fprintf(g,"\n");

	char *command;
	command=(char*)malloc(15*sizeof(char));//vector ce stocheaza comenzile pentru apelarea ulterioare

	fprintf(g,"Se vor face urmatoarele modificari in lista de procese:\n\n");
	for(int i=0;i<dim;i++)//parcurgere lista
	{	
		if(PID[i]!=0)//daca exista procesul in lista
		{
			for(int j=0;j<dim;j++)
			{	
				if(PID[i]==v[j])
				{
					fprintf(g,"Pentru procesul cu PID=%d ",v[j]);
					if(RES[i]>limita_res[j])
					{
						fprintf(g,"limitez RES.\n");
						//constructia comenzii si executarea acesteia in terminal
						//sprintf(command, "kill -19 %d",v[i]);
						//system(command);
					}
					else
						if(STARE[i]=='Z')//daca procesul este zombi
						{
				                        sprintf(command, "kill -19 %d",v[i]);
                                                        system(command);
							fprintf(g,"il scot din starea zombi.\n");
						}
						else
						{
							//variabile ce stocheaza informatiile sub forma de float in urma transformarilor pentru a evita eroarea de interpretare in cazul in care procentajele devin numere alcatuite din doua cifre
							float CPU_val, MEM_val, limita_cpu_val, limita_mem_val;
							
							//folosim functia atof pentru transformare de stringuri in float
							CPU_val=atof(CPU[i]);
							MEM_val=atof(MEM[i]);
							limita_cpu_val=atof(limita_cpu[j]);
							limita_mem_val=atof(limita_mem[j]);
							
							if(CPU_val>limita_cpu_val)
							{
					                        sprintf(command, "kill -19 %d",v[i]);
                                                        	system(command);
								fprintf(g,"limitez CPU.\n");
							}
							else
								if(MEM_val>limita_mem_val)
								{
									sprintf(command, "kill -19 %d",v[i]);
                                                        		system(command);
									fprintf(g,"limitez MEM\n");
								}
								else
									fprintf(g,"nu se va modifica nimic.\n");
						}
				}
			}
		}
	}
	fprintf(g,"----------------------------------------------------------------------");


	//eliberare memorie
	free(v);
	free(limita_res);
	free(limita_cpu);
	for(int i=0;i<dim;i++)
		free(limita_cpu[i]);
	free(limita_mem);
	for(int i=0;i<dim;i++)
                free(limita_mem[i]);
	free(merge);
	free(PID);
	free(RES);
	free(STARE);
	for(int i=0;i<dim;i++)
		free(STARE[i]);
	free(MEM);
	for(int i=0;i<dim;i++)
                free(MEM[i]);
	free(CPU);
        for(int i=0;i<dim;i++)
                free(CPU[i]);
	free(local_mem);
	free(local_cpu);
	free(local_stare);
	free(v_frecventa);
	free(command);

	//inchidere fisiere
	fclose(f);
	fclose(g);
	fclose(lung);
	fclose(pid);
	fclose(res);
	fclose(stare);
	fclose(mem);
	fclose(cpu);

	return 0;
}

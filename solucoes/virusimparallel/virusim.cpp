//  
// Simulação de propagação de vírus.
// Adaptada de um código proposto por David Joiner (Kean University).
//
// Uso: virusim <tamanho-da-populacao> <nro. experimentos> <probab. maxima> 

#include <cstdio>
#include <cstdlib>
#include <iostream>
#include "Random.h"
#include "Population.h"
#include <omp.h>

#define BSD_SOURCE
#include <sys/time.h>

void
checkCommandLine(int argc, char** argv, int& size, int& trials, int& probs, int& n_threads)
{
   if (argc > 1) {
      size = atoi(argv[1]);
   }   
   if (argc > 2) {
      trials = atoi(argv[2]);
   }
   if (argc > 3) {
      probs = atoi(argv[3]);
   }
   if(argc > 4) {
      n_threads = atoi(argv[4]);
   }   
}

long wtime()
{
   struct timeval t;
   gettimeofday(&t, NULL);
   return t.tv_sec*1000000 + t.tv_usec;
}

int 
main(int argc, char* argv[]) 
{
   
   // parâmetros dos experimentos
   long start_time, end_time;
   int population_size = 30;
   int n_trials = 5000;
   int n_probs = 101;

   double* percent_infected; // percentuais de infectados (saída)
   double* prob_spread;      // probabilidades a serem testadas (entrada)
   double prob_min = 0.0;
   double prob_max = 1.0;
   double prob_step;
   int base_seed = 100;
   int num_threads = 4;

   checkCommandLine(argc, argv, population_size, n_trials, n_probs, num_threads);

   omp_set_num_threads(num_threads);

   try {

      // Population* population = new Population(population_size);
      // Population* population = NULL;
      Random rand;

      prob_spread = new double[n_probs];
      percent_infected = new double[n_probs];

      prob_step = (prob_max - prob_min)/(double)(n_probs-1);

      printf("Probabilidade, Percentual Infectado\n");
      start_time = wtime();
      // para cada probabilidade, calcula o percentual de pessoas infectadas
      for (int ip = 0; ip < n_probs; ip++)
      {
         prob_spread[ip] = prob_min + (double)ip * prob_step;
         percent_infected[ip] = 0.0;
         // nova sequência de números aleatórios
         rand.setSeed(base_seed + ip);

         // executa vários experimentos para esta probabilidade
         #pragma omp parallel 
         {
            #pragma omp for schedule(static)
            for (int it = 0; it < n_trials; it++)
            {
                  Population *population = new Population(population_size);
                  // queima floresta até o fogo apagar
                  population->propagateUntilOut(population->centralPerson(), prob_spread[ip], rand);
                  #pragma omp critical
                  {
                     percent_infected[ip] += population->getPercentInfected();
                  }
            }
         }
         // calcula média dos percentuais de árvores queimadas
         percent_infected[ip] /= n_trials;

         // mostra resultado para esta probabilidade
         printf("%lf, %lf\n", prob_spread[ip], percent_infected[ip]);
      }

      end_time = wtime();
      printf("N Threads: %d Tempo(Usec): %ld\n", num_threads, long(end_time - start_time));

      delete[] prob_spread;
      delete[] percent_infected;
   }
   catch (std::bad_alloc)
   {
      std::cerr << "Erro: alocacao de memoria" << std::endl;
      return 1;
   }

   return 0;
}


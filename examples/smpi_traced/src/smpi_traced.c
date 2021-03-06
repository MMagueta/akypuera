/* Copyright (c) 2009, 2010. The SimGrid Team.
 * All rights reserved.                                                     */

/* This program is free software; you can redistribute it and/or modify it
 * under the terms of the license (GNU LGPL) which comes with this package. */

#include "mpi.h"
#include <stdio.h>
#include <stdlib.h>

#define DATATOSENT 100000

int main(int argc, char *argv[])
{
  int rank, numprocs, i;

  MPI_Init(&argc, &argv);
  MPI_Comm_size(MPI_COMM_WORLD, &numprocs);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);

  int A, B, C, D, E, F, G, H, I, J;
  //Tests: 
  //A: 0(isend -> wait) with 1(recv)
  A = 1;
  //B: 0(send) with 1(irecv -> wait)
  B = 1;
  //C: 0(N * isend -> N * wait) with 1(N * recv)
  C = 1;
  //D: 0(N * isend -> N * waitany) with 1(N * recv)
  D = 1;
  //E: 0(N*send) with 1(N*irecv, N*wait)
  E = 1;
  //F: 0(N*send) with 1(N*irecv, N*waitany)
  F = 1;
  //G: 0(N* isend -> waitall) with 1(N*recv)
  G = 1;
  //H: 0(N*send) with 1(N*irecv, waitall)
  H = 1;
  //I: 0(2*N*send, 2*N*Irecv, Waitall) with
  //   1(N*irecv, waitall, N*isend, N*waitany) with 
  //   2(N*irecv, N*waitany, N*isend, waitall)
  I = 1;
  //J: 0(N*isend, N*test, N*wait) with (N*irecv, N*test, N*wait)
  J = 0;

  int N = 13;

  int tag = 12345;
/////////////////////////////////////////
////////////////// RANK 0
///////////////////////////////////
  if (rank == 0) {
    MPI_Request request;
    MPI_Status status;
    MPI_Request req[2 * N];
    MPI_Status sta[2 * N];
    int *r = (int *) malloc(sizeof(int) * DATATOSENT);
    if (A) {
      MPI_Isend(r, DATATOSENT, MPI_INT, 1, tag, MPI_COMM_WORLD, &request);
      MPI_Wait(&request, &status);
    }
    MPI_Barrier(MPI_COMM_WORLD);

    if (B) {
      MPI_Send(r, DATATOSENT, MPI_INT, 1, tag, MPI_COMM_WORLD);
    }
    MPI_Barrier(MPI_COMM_WORLD);

    if (C) {
      for (i = 0; i < N; i++) {
        MPI_Isend(r, DATATOSENT, MPI_INT, 1, tag, MPI_COMM_WORLD, &req[i]);
      }
      for (i = 0; i < N; i++) {
        MPI_Wait(&req[i], &sta[i]);
      }
    }
    MPI_Barrier(MPI_COMM_WORLD);

    if (D) {
      for (i = 0; i < N; i++) {
        MPI_Isend(r, DATATOSENT, MPI_INT, 1, tag, MPI_COMM_WORLD, &req[i]);
      }
      for (i = 0; i < N; i++) {
        int completed;
        MPI_Waitany(N, req, &completed, sta);
      }
    }
    MPI_Barrier(MPI_COMM_WORLD);

    if (E) {
      for (i = 0; i < N; i++) {
        MPI_Send(r, DATATOSENT, MPI_INT, 1, tag, MPI_COMM_WORLD);
      }
    }
    MPI_Barrier(MPI_COMM_WORLD);

    if (F) {
      for (i = 0; i < N; i++) {
        MPI_Send(r, DATATOSENT, MPI_INT, 1, tag, MPI_COMM_WORLD);
      }
    }
    MPI_Barrier(MPI_COMM_WORLD);

    if (G) {
      for (i = 0; i < N; i++) {
        MPI_Isend(r, DATATOSENT, MPI_INT, 1, tag, MPI_COMM_WORLD, &req[i]);
      }
      MPI_Waitall(N, req, sta);
    }
    MPI_Barrier(MPI_COMM_WORLD);

    if (H) {
      for (i = 0; i < N; i++) {
        MPI_Send(r, DATATOSENT, MPI_INT, 1, tag, MPI_COMM_WORLD);
      }
    }
    MPI_Barrier(MPI_COMM_WORLD);

    if (I) {
      for (i = 0; i < 2 * N; i++) {
        if (i < N) {
          MPI_Send(r, DATATOSENT, MPI_INT, 2, tag, MPI_COMM_WORLD);
        } else {
          MPI_Send(r, DATATOSENT, MPI_INT, 1, tag, MPI_COMM_WORLD);
        }
      }
      MPI_Barrier(MPI_COMM_WORLD);
      for (i = 0; i < 2 * N; i++) {
        if (i < N) {
          MPI_Irecv(r, DATATOSENT, MPI_INT, 1, tag, MPI_COMM_WORLD,
                    &req[i]);
        } else {
          MPI_Irecv(r, DATATOSENT, MPI_INT, 2, tag, MPI_COMM_WORLD,
                    &req[i]);
        }
      }
      MPI_Waitall(2 * N, req, sta);
    }
    MPI_Barrier(MPI_COMM_WORLD);

    if (J) {
      for (i = 0; i < N; i++) {
        MPI_Isend(r, DATATOSENT, MPI_INT, 1, tag, MPI_COMM_WORLD, &req[i]);
      }
      for (i = 0; i < N; i++) {
        int flag;
        MPI_Test(&req[i], &flag, &sta[i]);
      }
      for (i = 0; i < N; i++) {
        MPI_Wait(&req[i], &sta[i]);
      }
    }
/////////////////////////////////////////
////////////////// RANK 1
///////////////////////////////////
  } else if (rank == 1) {
    MPI_Request request;
    MPI_Status status;
    MPI_Request req[N];
    MPI_Status sta[N];
    int *r = (int *) malloc(sizeof(int) * DATATOSENT);

    if (A) {
      MPI_Recv(r, DATATOSENT, MPI_INT, 0, tag, MPI_COMM_WORLD, &status);
    }
    MPI_Barrier(MPI_COMM_WORLD);

    if (B) {
      MPI_Irecv(r, DATATOSENT, MPI_INT, 0, tag, MPI_COMM_WORLD, &request);
      MPI_Wait(&request, &status);
    }
    MPI_Barrier(MPI_COMM_WORLD);

    if (C) {
      for (i = 0; i < N; i++) {
        MPI_Recv(r, DATATOSENT, MPI_INT, 0, tag, MPI_COMM_WORLD, &sta[i]);
      }
    }
    MPI_Barrier(MPI_COMM_WORLD);

    if (D) {
      for (i = 0; i < N; i++) {
        MPI_Recv(r, DATATOSENT, MPI_INT, 0, tag, MPI_COMM_WORLD, &sta[i]);
      }
    }
    MPI_Barrier(MPI_COMM_WORLD);

    if (E) {
      for (i = 0; i < N; i++) {
        MPI_Irecv(r, DATATOSENT, MPI_INT, 0, tag, MPI_COMM_WORLD, &req[i]);
      }
      for (i = 0; i < N; i++) {
        MPI_Wait(&req[i], &sta[i]);
      }
    }
    MPI_Barrier(MPI_COMM_WORLD);

    if (F) {
      for (i = 0; i < N; i++) {
        MPI_Irecv(r, DATATOSENT, MPI_INT, 0, tag, MPI_COMM_WORLD, &req[i]);
      }
      for (i = 0; i < N; i++) {
        int completed;
        MPI_Waitany(N, req, &completed, sta);
      }
    }
    MPI_Barrier(MPI_COMM_WORLD);

    if (G) {
      for (i = 0; i < N; i++) {
        MPI_Recv(r, DATATOSENT, MPI_INT, 0, tag, MPI_COMM_WORLD, &sta[i]);
      }
    }
    MPI_Barrier(MPI_COMM_WORLD);

    if (H) {
      for (i = 0; i < N; i++) {
        MPI_Irecv(r, DATATOSENT, MPI_INT, 0, tag, MPI_COMM_WORLD, &req[i]);
      }
      MPI_Waitall(N, req, sta);
    }
    MPI_Barrier(MPI_COMM_WORLD);

    if (I) {
      for (i = 0; i < N; i++) {
        MPI_Irecv(r, DATATOSENT, MPI_INT, 0, tag, MPI_COMM_WORLD, &req[i]);
      }
      MPI_Waitall(N, req, sta);

      MPI_Barrier(MPI_COMM_WORLD);
      for (i = 0; i < N; i++) {
        MPI_Isend(r, DATATOSENT, MPI_INT, 0, tag, MPI_COMM_WORLD, &req[i]);
      }
      MPI_Waitall(N, req, sta);
//      for (i = 0; i < N; i++){
//        MPI_Wait (&req[i], &sta[i]);
//      }
    }
    MPI_Barrier(MPI_COMM_WORLD);

    if (J) {
      for (i = 0; i < N; i++) {
        MPI_Irecv(r, DATATOSENT, MPI_INT, 0, tag, MPI_COMM_WORLD, &req[i]);
      }
      for (i = 0; i < N; i++) {
        int flag;
        MPI_Test(&req[i], &flag, &sta[i]);
      }
      for (i = 0; i < N; i++) {
        MPI_Wait(&req[i], &sta[i]);
      }
    }
/////////////////////////////////////////
////////////////// RANK 2
///////////////////////////////////
  } else if (rank == 2) {
//    MPI_Request request;
//    MPI_Status status;
    MPI_Request req[N];
    MPI_Status sta[N];
    int *r = (int *) malloc(sizeof(int) * DATATOSENT);

    if (A) {
    }
    MPI_Barrier(MPI_COMM_WORLD);
    if (B) {
    }
    MPI_Barrier(MPI_COMM_WORLD);
    if (C) {
    }
    MPI_Barrier(MPI_COMM_WORLD);
    if (D) {
    }
    MPI_Barrier(MPI_COMM_WORLD);
    if (E) {
    }
    MPI_Barrier(MPI_COMM_WORLD);
    if (F) {
    }
    MPI_Barrier(MPI_COMM_WORLD);
    if (G) {
    }
    MPI_Barrier(MPI_COMM_WORLD);
    if (H) {
    }
    MPI_Barrier(MPI_COMM_WORLD);
    if (I) {
      for (i = 0; i < N; i++) {
        MPI_Irecv(r, DATATOSENT, MPI_INT, 0, tag, MPI_COMM_WORLD, &req[i]);
      }
      for (i = 0; i < N; i++) {
        int completed;
        MPI_Waitany(N, req, &completed, sta);
      }
      MPI_Barrier(MPI_COMM_WORLD);

      for (i = 0; i < N; i++) {
        MPI_Send(r, DATATOSENT, MPI_INT, 0, tag, MPI_COMM_WORLD);
      }
    }
    MPI_Barrier(MPI_COMM_WORLD);
    if (J) {
    }
  }
  MPI_Barrier(MPI_COMM_WORLD);
  MPI_Finalize();
  return 0;
}

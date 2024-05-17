# Minimal reproducer for Carthesian Product error with nvc++

I may have found an error in the nvc++ compiler. It seems that when you use the
carthesian product and mdspan to modify data with par_unseq policy and `-stdpar=gpu|multicore`, you end up with data corruption.
The problem disappear when you use `-nostdpar` or if you use the seq policy.

## Description

In [main.cpp](main.cpp), we have a sequential reference function and one using the STL and the carthesian product. The other functions are only here for utils (printing and stuff).

## Reproduce the bug

```console
make run
```

## Compiler version

The error is present at least with nvc++ 24.1 and 24.3.

24.1 :
```console
> nvc++ -V

nvc++ 24.1-0 64-bit target on x86-64 Linux -tp haswell 
NVIDIA Compilers and Tools
Copyright (c) 2024, NVIDIA CORPORATION & AFFILIATES.  All rights reserved.
```

24.3 :
```console
> nvc++ -V

nvc++ 24.3-0 64-bit target on x86-64 Linux -tp haswell 
NVIDIA Compilers and Tools
Copyright (c) 2024, NVIDIA CORPORATION & AFFILIATES.  All rights reserved.
```

## Output Example

WITH NVC++ USING GPU FOR PARALLEL EXECUTION
------------ Reference ------------      /         ------------ Result ------------

  0   0   0   0   0   0          /         0   0   0   0   0   0 
  0   0   0   0   0   0          /         0   0   0   0   0   0 
  0   0   0   0   0   0          /         0   0   0   0   0   0 
  0   0   0   0   0   0          /         0   0   0   0   0   0 
  0   0   0   0   0   0          /         0   0   0   0   0   0 

  0   0   0   0   0   0          /         0   0   0   0   0   0 
  0   0   0   0   0   0          /         0   0   0   0   0   0 
  0 155.5 156.5 157.5   0   0    /         0 155.5 156.5   0   0   0 
  0 161.5 162.5 163.5   0   0    /         0 161.5   0 163.5   0   0 
  0   0   0   0   0   0          /         0   0 154.594 155.531   0   0 

  0   0   0   0   0   0          /         0   0   0   0   0   0 
  0   0   0   0   0   0          /         0   0   0   0   0   0 
  0   0   0   0   0   0          /         0   0   0   0   0   0 
  0   0   0   0   0   0          /         0   0   0   0   0   0 
  0   0   0   0   0   0          /         0   0   0   0   0   0 

  0   0   0   0   0   0          /         0   0   0   0   0   0 
  0   0   0   0   0   0          /         0   0   0   0   0   0 
  0   0   0   0   0   0          /         0   0   0   0   0   0 
  0   0   0   0   0   0          /         0   0   0   0   0   0 
  0   0   0   0   0   0          /         0   0   0   0   0   0 

WITH G++ USING TBB FOR PARALLEL EXECUTION
------------ Reference ------------      /         ------------ Result ------------

  0   0   0   0   0   0          /         0   0   0   0   0   0 
  0   0   0   0   0   0          /         0   0   0   0   0   0 
  0   0   0   0   0   0          /         0   0   0   0   0   0 
  0   0   0   0   0   0          /         0   0   0   0   0   0 
  0   0   0   0   0   0          /         0   0   0   0   0   0 

  0   0   0   0   0   0          /         0   0   0   0   0   0 
  0   0   0   0   0   0          /         0   0   0   0   0   0 
  0 155.5 156.5 157.5   0   0    /         0 155.5 156.5 157.5   0   0 
  0 161.5 162.5 163.5   0   0    /         0 161.5 162.5 163.5   0   0 
  0   0   0   0   0   0          /         0   0   0   0   0   0 

  0   0   0   0   0   0          /         0   0   0   0   0   0 
  0   0   0   0   0   0          /         0   0   0   0   0   0 
  0   0   0   0   0   0          /         0   0   0   0   0   0 
  0   0   0   0   0   0          /         0   0   0   0   0   0 
  0   0   0   0   0   0          /         0   0   0   0   0   0 

  0   0   0   0   0   0          /         0   0   0   0   0   0 
  0   0   0   0   0   0          /         0   0   0   0   0   0 
  0   0   0   0   0   0          /         0   0   0   0   0   0 
  0   0   0   0   0   0          /         0   0   0   0   0   0 
  0   0   0   0   0   0          /         0   0   0   0   0   0
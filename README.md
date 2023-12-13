## Building the Project

These instructions work for building on the class server. There is also an existing build of the project that can be found on 583a at `/home/leshlu/final-project`. 

### S-Detector 
The following instructions assume starting in the root directory of the project as the working directory.

```
cd S-Detector
cd MASBasedPass
mkdir build && cd build
cmake ..
make
```

The Naive pass must also be compiled for running the tests...
```
cd S-Detector
cd NaivePass
mkdir build && cd build
cmake ..
make
```

The tests may then be run from the `S-Detector/benchmarks` directory. For example... 

```
chmod +x run.sh
./run.sh test1
```

Be sure to not include the .c when running a given test. 

### S-Tracer
Start with the root directory of the project as your working directory.

```
cd S-Tracer
mkdir build && cd build
cmake ..
make
```

Examples may then be run with the `S-Tracer/run.sh` script similarly to S-Detector, with the keyword argument being an input file. In this case, the demo included `bubblesort` and `patterns`. 
